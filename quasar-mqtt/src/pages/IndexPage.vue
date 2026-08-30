<template>
  <q-page class="bg-grey-3">
    <div class="q-pa-md">
      <div class="row items-center q-mb-md">
        <q-chip color="primary" text-color="white" icon="person" clickable @click="valjNamn">
          Du är: {{ me || '…' }}
        </q-chip>
        <q-chip :color="connected ? 'positive' : 'warning'" :text-color="connected ? 'white' : 'dark'" :icon="connected ? 'cloud_done' : 'cloud_off'">
          {{ connected ? 'Ansluten' : 'Ansluter…' }}
        </q-chip>
        <span class="text-caption text-grey-7 q-ml-sm">{{ TOPIC }}</span>
      </div>

      <q-chat-message
        v-for="(info, i) in chatMessages"
        :key="i"
        :name="info.user"
        :avatar="avatarFor(info.user)"
        :text="[info.message]"
        :sent="info.user === me"
        :bg-color="info.user === me ? 'primary' : 'white'"
        :text-color="info.user === me ? 'white' : 'dark'"
      />
    </div>

    <div class="absolute-bottom row q-pa-sm bg-white">
      <q-input
        v-model="publishMessage"
        class="col"
        outlined
        dense
        placeholder="Skriv ett meddelande (on / off / status styr lampan)"
        @keyup.enter="publish"
      />
      <q-btn class="q-ml-sm" icon="send" color="primary" :disable="!connected || !me" @click="publish" />
    </div>
  </q-page>
</template>

<script setup>
import { ref, onMounted, onUnmounted } from 'vue'
import { useQuasar, SessionStorage } from 'quasar'
import { client, TOPIC } from '@/boot/mqtt-boot'

const $q = useQuasar()

// Quasars SessionStorage-plugin = webbläsarens sessionStorage, som är PER FLIK.
// Varje ny flik blir därför en egen användare. (LocalStorage-pluginen hade
// delats mellan alla flikar – då hade alla blivit samma användare.)
const NAMN_NYCKEL = 'chatt-namn'
function sparatNamn () {
  return SessionStorage.getItem(NAMN_NYCKEL) || ''
}
function sparaNamn (namn) {
  SessionStorage.set(NAMN_NYCKEL, namn)
}
function slumpnamn () {
  const djur = ['räv', 'uggla', 'igelkott', 'älg', 'lo', 'bäver', 'ekorre', 'trana', 'säl', 'varg']
  return djur[Math.floor(Math.random() * djur.length)] + '-' + Math.floor(Math.random() * 90 + 10)
}

const me = ref(sparatNamn())
const connected = ref(client.connected)
const publishMessage = ref('')
const chatMessages = ref([])

function valjNamn () {
  $q.dialog({
    title: 'Vem är du i chatten?',
    message: 'Namnet visas för alla på samma topic. Varje flik är en egen användare.',
    prompt: { model: me.value || slumpnamn(), type: 'text', isValid: v => v.trim().length > 0 },
    cancel: !!me.value,
    persistent: true,
  }).onOk(namn => {
    const nytt = namn.trim()
    const bytte = me.value && me.value !== nytt
    me.value = nytt
    sparaNamn(nytt)
    if (client.connected) send(bytte ? 'bytte namn och är här' : 'har anslutit')
  })
}

// Samma namn ger alltid samma avatar (6 st finns på cdn.quasar.dev)
function avatarFor (user) {
  let h = 0
  for (const c of String(user)) h = (h * 31 + c.charCodeAt(0)) >>> 0
  return `https://cdn.quasar.dev/img/avatar${(h % 6) + 1}.jpg`
}

function onConnect () {
  connected.value = true
  client.subscribe(TOPIC, err => {
    if (!err && me.value) send('har anslutit')
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
  client.publish(TOPIC, JSON.stringify({ user: me.value, message: text }))
}

function publish () {
  if (!publishMessage.value.trim() || !me.value) return
  send(publishMessage.value)
  publishMessage.value = ''
}

onMounted(() => {
  client.on('connect', onConnect)
  client.on('close', onClose)
  client.on('message', onMessage)
  if (client.connected) onConnect()
  if (!me.value) valjNamn()
})

onUnmounted(() => {
  client.off('connect', onConnect)
  client.off('close', onClose)
  client.off('message', onMessage)
})
</script>
