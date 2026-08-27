import { defineBoot } from '#q-app'
import mqtt from 'mqtt'

// Skolans MQTT-broker (Eclipse Mosquitto på CapRover). Ingen inloggning krävs.
//   Webbläsare, http-sida:   ws://mqtt-broker.cloud.mustini.com:9001
//   Webbläsare, https-sida:  wss://mqtt-broker.cloud.mustini.com
//   ESP8266 / mosquitto_pub: mqtt-broker.cloud.mustini.com:1883
// En https-sida får inte öppna ws:// (mixed content), därför väljs adress efter sidans protokoll.
export const BROKER_URL = window.location.protocol === 'https:'
  ? 'wss://mqtt-broker.cloud.mustini.com'
  : 'ws://mqtt-broker.cloud.mustini.com:9001'

// Byt till ditt eget prefix så att du inte krockar med andra på brokern.
export const TOPIC = 'abbjetmus/chat'

export const client = mqtt.connect(BROKER_URL, {
  // Unikt id per flik – två klienter med samma id sparkar ut varandra från brokern.
  clientId: 'salle-' + Math.random().toString(16).slice(2, 8),
  clean: true,
  reconnectPeriod: 2000,
})

client.on('connect', () => console.log('MQTT ansluten till', BROKER_URL))
client.on('reconnect', () => console.log('MQTT återansluter...'))
client.on('error', (err) => console.error('MQTT-fel:', err.message))

export default defineBoot(({ app }) => {
  // Gör klienten nåbar som this.$mqtt i Options API-komponenter också
  app.config.globalProperties.$mqtt = client
})
