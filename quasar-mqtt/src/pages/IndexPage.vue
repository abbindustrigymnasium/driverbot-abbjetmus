<template>
  <q-page class="bg-grey-3">
    <div class="q-pa-md">
      <q-banner v-if="!connected" dense class="bg-warning text-dark q-mb-md">
        Ansluter till {{ BROKER_URL }} …
      </q-banner>

      <q-chat-message
        v-for="(info, i) in chatMessages"
        :key="i"
        :name="info.user"
        :avatar="info.user === ME
          ? 'https://cdn.quasar.dev/img/avatar1.jpg'
          : 'https://cdn.quasar.dev/img/avatar2.jpg'"
        :text="[info.message]"
        :sent="info.user === ME"
        :bg-color="info.user === ME ? 'primary' : 'positive'"
        :text-color="info.user === ME ? 'white' : 'dark'"
      />
    </div>

    <div class="absolute-bottom row q-pa-sm bg-white">
      <q-input
        v-model="publishMessage"
        class="col"
        outlined
        dense
        placeholder="Skriv ett meddelande"
        @keyup.enter="publish"
      />
      <q-btn class="q-ml-sm" icon="send" color="primary" :disable="!connected" @click="publish" />
    </div>
  </q-page>
</template>

<script setup>
import { ref, onMounted, onUnmounted } from 'vue'
import { client, TOPIC, BROKER_URL } from '@/boot/mqtt-boot'

const ME = 'salle'

const connected = ref(client.connected)
const publishMessage = ref('')
const chatMessages = ref([])

function onConnect () {
  connected.value = true
  client.subscribe(TOPIC, (err) => {
    if (!err) send('Hej från Quasar!')
  })
}

function onClose () {
  connected.value = false
}

function onMessage (topic, message) {
  console.log(`${topic} - ${message.toString()}`)
  try {
    chatMessages.value.push(JSON.parse(message.toString()))
  } catch {
    chatMessages.value.push({ user: topic, message: message.toString() })
  }
}

function send (text) {
  client.publish(TOPIC, JSON.stringify({ user: ME, message: text }))
}

function publish () {
  if (!publishMessage.value.trim()) return
  send(publishMessage.value)
  publishMessage.value = ''
}

onMounted(() => {
  client.on('connect', onConnect)
  client.on('close', onClose)
  client.on('message', onMessage)
  if (client.connected) onConnect()
})

onUnmounted(() => {
  client.off('connect', onConnect)
  client.off('close', onClose)
  client.off('message', onMessage)
})
</script>
