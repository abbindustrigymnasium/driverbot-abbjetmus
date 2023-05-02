<template>
  <q-page class="bg-grey-3">
    <div class="q-pa-md">
      <div style="width: 100%">
        <q-chat-message v-for="info in chatMessages" :name="info.user" :avatar="info.user == 'salle'
            ? 'https://cdn.quasar.dev/img/avatar1.jpg'
            : 'https://cdn.quasar.dev/img/avatar2.jpg'
          " :text="[info.message]" :sent="info.user == 'salle'"
          :bg-color="info.user == 'salle' ? 'primary' : 'success'" />
      </div>
    </div>
    <div class="absolute-bottom row">
      <q-input type="text" class="col bg-white" v-model="publishMessage" outlined />
      <q-btn @click="publish" icon="send" color="primary"></q-btn>
    </div>
  </q-page>
</template>

<script setup>
import { client } from "src/boot/mqtt-boot"
import { ref, onMounted } from 'vue'

onMounted(() => {
  client.on("connect", () => {
    console.log("Conntected!")
    client.subscribe("topic", function (err) {
      if (!err) {
        let info = JSON.stringify({
          user: "salle",
          message: "Hello mqtt",
        })
        client.publish("topic", info)
      }
    })
  })
})

client.on("message", (topic, message) => {
  console.log(`${topic} - ${message.toString()}`)
  let info = JSON.parse(message)
  chatMessages.value.push(info)
})

const publishMessage = ref("")
const chatMessages = ref([])

const publish = () => {
  let info = JSON.stringify({
    user: "salle",
    message: publishMessage.value,
  })
  client.publish("topic", info)
  publishMessage.value = ""
}

</script>
