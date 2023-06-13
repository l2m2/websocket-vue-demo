#include <iostream>
#include <uwebsockets/App.h>

int main()
{
    /* ws->getUserData returns one of these */
    struct PerSocketData {
        /* Fill with user data */
        std::string* userName;
    };

    /* Keep in mind that uWS::SSLApp({options}) is the same as uWS::App() when compiled without SSL support.
     * You may swap to using uWS:App() if you don't need SSL */
    uWS::App().ws<PerSocketData>("/*", {
        /* Settings */
        .compression = uWS::CompressOptions(uWS::DEDICATED_COMPRESSOR_4KB | uWS::DEDICATED_DECOMPRESSOR),
        .maxPayloadLength = 100 * 1024 * 1024,
        .idleTimeout = 16,
        .maxBackpressure = 100 * 1024 * 1024,
        .closeOnBackpressureLimit = false,
        .resetIdleTimeoutOnSend = false,
        .sendPingsAutomatically = true,
        /* Handlers */
        .upgrade = [](auto* res, auto* req, auto* context) {
            res->template upgrade<PerSocketData>({
                /* We initialize PerSocketData struct here */
                .userName = new std::string(req->getQuery("user"))
            }, req->getHeader("sec-websocket-key"), 
                req->getHeader("sec-websocket-protocol"),
                req->getHeader("sec-websocket-extensions"),
                context);
        },
        .open = [](auto* ws) {
            /* Open event here, you may access ws->getUserData() which points to a PerSocketData struct */
            PerSocketData* data = (PerSocketData*)ws->getUserData();
            std::cout << "open: " << *data->userName << std::endl;
        },
        .message = [](auto* ws, std::string_view message, uWS::OpCode opCode) {
            /* This is the opposite of what you probably want; compress if message is LARGER than 16 kb
             * the reason we do the opposite here; compress if SMALLER than 16 kb is to allow for
             * benchmarking of large message sending without compression */
            if (message == "love") {
                message = "sorry i don't love u.";
            }
            ws->send(message, opCode, message.length() < 16 * 1024);
        },
        .drain = [](auto*/*ws*/) {
            /* Check ws->getBufferedAmount() here */
        },
        .ping = [](auto*/*ws*/, std::string_view) {
            /* Not implemented yet */
            std::cout << "ping" << std::endl;
        },
        .pong = [](auto*/*ws*/, std::string_view) {
            /* Not implemented yet */
            std::cout << "pong" << std::endl;
        },
        .close = [](auto* ws, int /*code*/, std::string_view /*message*/) {
            /* You may access ws->getUserData() here */
            PerSocketData* data = (PerSocketData*)ws->getUserData();
            std::cout << "close: " << *data->userName << std::endl;
            delete data->userName;
        }
        }).listen(9001, [](auto* listen_socket) {
            if (listen_socket) {
                std::cout << "Listening on port " << 9001 << std::endl;
            }
            }).run();
}