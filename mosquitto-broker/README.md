# mosquitto-broker — skolans MQTT-broker

Det här är konfigurationen för `mqtt-broker.cloud.mustini.com`: en **Eclipse Mosquitto 2** som körs som en app i CapRover. Samma filer kan användas för att köra en egen broker lokalt med Docker.

| Port | Protokoll | Klienter |
|---|---|---|
| 1883 | MQTT över TCP | ESP8266, `mosquitto_pub/sub`, Python, ROS2-bryggan |
| 9001 | MQTT över WebSockets | Webbsidor på http (`ws://…:9001`) |
| 443 | MQTT över WebSockets + TLS | Webbsidor på https (`wss://…`) — CapRovers nginx proxar till 9001 |

Ingen inloggning krävs (`allow_anonymous true`).

## Kör en egen broker lokalt (Docker)

```bash
docker build -t min-mosquitto .
docker run --rm -p 1883:1883 -p 9001:9001 min-mosquitto
```

Testa med två terminaler:

```bash
mosquitto_sub -h localhost -t "test/#" -v
mosquitto_pub -h localhost -t test/hej -m "hej"
```

## Deploya till CapRover

1. Skapa en app (t.ex. `mqtt-broker`) med *persistent data*.
2. App Configs → **Port Mapping**: `1883 → 1883` och `9001 → 9001`. Volym `mqtt-broker-data → /mosquitto/data`.
3. För `wss://`: HTTP-port (container) = `9001`, *Websocket Support* på, aktivera HTTPS.
4. `tar -cf deploy.tar captain-definition Dockerfile mosquitto.conf && caprover deploy -t ./deploy.tar`
