// mqtt_arduino.ino — ESP8266 som chattar med Quasar-appen via skolans MQTT-broker
//
// Bibliotek (Arduino IDE -> Library Manager): "EspMQTTClient" (drar in PubSubClient).
// Kort: ESP8266 (NodeMCU / Wemos D1 mini). Välj rätt kort under Tools -> Board.
//
// Så funkar det:
//   - ESP:n ansluter till skolans broker och prenumererar på samma topic som webbappen.
//   - Varje meddelande i chatten -> inbyggda lampan blinkar i 2 sekunder.
//   - Skriv "on" eller "off" i chatten -> lampan tänds/släcks permanent.
//   - ESP:n svarar i chatten som användaren "esp8266" så att du ser att den lever.
//
// Broker:  mqtt-broker.cloud.mustini.com, port 1883, ingen inloggning.
// Topic:   samma som TOPIC i quasar-mqtt/src/boot/mqtt-boot.js (byt till ditt eget prefix!)

#include "EspMQTTClient.h"

// ==================== ANPASSA ====================
#define WIFI_SSID     "Hitachigymnasiet_2.4"
#define WIFI_PASSWORD "..."
#define MQTT_BROKER   "mqtt-broker.cloud.mustini.com"
#define MQTT_PORT     1883
#define CLIENT_NAME   "esp8266-abbjetmus"      // måste vara unikt på brokern – byt till ditt namn
#define TOPIC         "abbjetmus/chat"         // samma som i webbappen

#define LED_PIN       LED_BUILTIN               // inbyggd lampa. Extern lampa: t.ex. D1
#define LED_ACTIVE_LOW true                     // inbyggda lampan på ESP8266 lyser vid LOW
// =================================================

EspMQTTClient client(
  WIFI_SSID, WIFI_PASSWORD,
  MQTT_BROKER,
  "", "",                                       // användarnamn/lösenord: inget krävs
  CLIENT_NAME,
  MQTT_PORT
);

bool lampaPa = false;                 // grundläge, styrs med on/off i chatten
unsigned long blinkTill = 0;          // lampan blinkar fram till denna tidpunkt
const unsigned long BLINK_MS = 2000;  // hur länge lampan lyser per meddelande

// Lampan lyser om grundläget är PÅ eller om vi är mitt i en meddelande-blink.
// Ingen delay() någonstans – client.loop() måste få köra hela tiden.
void uppdateraLampa() {
  bool tand = lampaPa || millis() < blinkTill;
  digitalWrite(LED_PIN, (tand != LED_ACTIVE_LOW) ? HIGH : LOW);
}

void sattLampa(bool pa) {
  lampaPa = pa;
  uppdateraLampa();
  Serial.println(pa ? "Lampan PÅ" : "Lampan AV");
}

// Skicka ett meddelande som dyker upp som en chattbubbla i webbappen.
// Webbappen förväntar sig JSON: {"user": "...", "message": "..."}
void skickaTillChatten(const String& text) {
  String json = "{\"user\":\"esp8266\",\"message\":\"" + text + "\"}";
  client.publish(TOPIC, json);
}

// Plocka ut ett fält ur JSON utan bibliotek (räcker för enkla strängar utan citattecken i).
String hamtaFalt(const String& json, const String& falt) {
  String nyckel = "\"" + falt + "\":\"";
  int start = json.indexOf(nyckel);
  if (start < 0) return "";
  start += nyckel.length();
  int slut = json.indexOf("\"", start);
  return slut < 0 ? "" : json.substring(start, slut);
}

void onConnectionEstablished() {
  Serial.println("MQTT ansluten till " MQTT_BROKER);

  client.subscribe(TOPIC, [](const String& payload) {
    blinkTill = millis() + BLINK_MS;   // varje meddelande -> lys i 2 sekunder

    String text = hamtaFalt(payload, "message");
    String anvandare = hamtaFalt(payload, "user");

    // Skriv ut meddelandet i Serial Monitor (115200 baud)
    if (text.length() > 0) {
      Serial.println("[chatt] " + anvandare + ": " + text);
    } else {
      Serial.println("[chatt] okant format: " + payload);
    }

    if (payload.indexOf("\"user\":\"esp8266\"") >= 0) return;   // ignorera våra egna meddelanden

    text.toLowerCase();
    if (text == "on" || text == "på") {
      sattLampa(true);
      skickaTillChatten("Lampan är PÅ");
    } else if (text == "off" || text == "av") {
      sattLampa(false);
      skickaTillChatten("Lampan är AV");
    } else if (text == "status") {
      skickaTillChatten(lampaPa ? "Lampan är PÅ" : "Lampan är AV");
    }
  });

  skickaTillChatten("ESP8266 online. Skriv on / off / status.");
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  sattLampa(false);
  client.enableDebuggingMessages();             // WiFi/MQTT-status i Serial Monitor
}

void loop() {
  client.loop();
  uppdateraLampa();
}
