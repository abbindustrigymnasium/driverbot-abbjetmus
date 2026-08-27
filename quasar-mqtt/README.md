# quasar-mqtt — Driverbot MQTT-chat

Quasar-app (Vue 3, `@quasar/app-vite` v3) som chattar över skolans MQTT-broker med [MQTT.js](https://github.com/mqttjs/MQTT.js) v5.

## Kör

Kräver Node 22.12 eller nyare.

```bash
npm install
npm run dev      # http://localhost:9000
npm run build    # dist/spa
```

## Broker

Adress och topic ändras i [src/boot/mqtt-boot.js](src/boot/mqtt-boot.js):

| Sidan körs över | Adress som väljs automatiskt |
|---|---|
| http (t.ex. `quasar dev`) | `ws://mqtt-broker.cloud.mustini.com:9001` |
| https (t.ex. GitHub Pages) | `wss://mqtt-broker.cloud.mustini.com` |

Byt `TOPIC` (`abbjetmus/chat`) till ditt eget prefix. Brokern är gemensam för hela skolan.

> **CSP:** `index.html` har en Content-Security-Policy. Byter du broker måste du lägga till den nya adressen under `connect-src`, annars blockerar webbläsaren anslutningen utan tydligt fel.

## Struktur

- `src/boot/mqtt-boot.js` — skapar MQTT-klienten en gång vid start, exporterar `client`, `TOPIC`, `BROKER_URL`
- `src/pages/IndexPage.vue` — chatten: prenumererar, publicerar, visar meddelanden
- `src/layouts/MainLayout.vue` — rubrik och länkar
