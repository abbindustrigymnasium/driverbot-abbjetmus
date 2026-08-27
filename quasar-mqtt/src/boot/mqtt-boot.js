import mqtt from "mqtt/dist/mqtt";

// Skolans MQTT-broker (Eclipse Mosquitto på CapRover). Ingen inloggning krävs.
//   Webbläsare (WebSockets):  ws://mqtt-broker.cloud.mustini.com:9001
//   ESP8266 / mosquitto_pub:  mqtt-broker.cloud.mustini.com:1883
// OBS: körs sidan över https måste adressen vara wss://, annars blockerar
// webbläsaren anslutningen (mixed content).
export const BROKER_URL = "ws://mqtt-broker.cloud.mustini.com:9001";

// Byt till ditt eget prefix så att du inte krockar med andra på brokern.
export const TOPIC = "abbjetmus/chat";

const options = {
  // Unikt id per flik – två klienter med samma id sparkar ut varandra från brokern.
  clientId: "salle-" + Math.random().toString(16).slice(2, 8),
  clean: true,
  reconnectPeriod: 2000,
};

const client = mqtt.connect(BROKER_URL, options);

client.on("connect", () => console.log("MQTT ansluten till", BROKER_URL));
client.on("reconnect", () => console.log("MQTT återansluter..."));
client.on("error", (err) => console.error("MQTT-fel:", err.message));

export { client };
