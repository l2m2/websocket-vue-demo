<script setup>
import { ref, onMounted } from 'vue';

const inputData = ref()
const messages = ref([])
const socket = new WebSocket("ws://localhost:9001/ws")
const submit = () => {
  sendMessage(inputData.value)
}
const sendMessage = (msg) => {
  socket.send(msg)
  messages.value.push(msg)
}
onMounted(() => {
  socket.onopen = function(){
    console.log("WebSocket connection opened");
  },
  socket.onmessage = function(event){
    console.log("onmessage: ", event);
    messages.value.push(event.data);
  },
  socket.onerror = function(error) {
    console.log("WebSocket error:", error);
  },
  socket.onclose = function(event)  {
    console.log("WebSocket connection closed:", event.code);
  }
})
</script>

<template>
  <div>
    <p>WebSocket Vue3 Demo</p>
    <input type="text" v-model="inputData" @keyup.enter="submit()" />
    <button @click="submit()">submit</button>
    <div class="messages-container">
      <div v-for="(val, index) in messages" :key="index" :class="[index % 2? 'left-bubble' : 'right-bubble']">
        <em>{{ val }}</em>
      </div>
    </div>
  </div>
  
</template>

<style scoped>
.left-bubble {
  color: red
}
.right-bubble {
  color: green
}
</style>
