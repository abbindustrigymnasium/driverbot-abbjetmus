#include "EspMQTTClient.h"
//Install libraries PubSubClient and EspMQTTClient


void onConnectionEstablished();

//Gamla sättet
/*EspMQTTClient client(
 "Nätverksnamn",           // Wifi ssid
  "password",           // Wifi password
  "mqtt-broker.cloud.mustini.com",  // MQTT broker (skolans, ingen inloggning)
  1883,             // MQTT broker port
  "",               // MQTT username (lämna tomt)
  "",               // MQTT password (lämna tomt)
  "klientnamn",          // Client name
  onConnectionEstablished, // Connection established callback
  true,             // Enable web updater
  true              // Enable debug messages
);*/
//Nya sättet!
EspMQTTClient client(
 "Nätverksnamn",           // Wifi ssid
  "password",           // Wifi password
  "mqtt-broker.cloud.mustini.com",  // MQTT broker (skolans, ingen inloggning)
  "",               // MQTT username (lämna tomt)
  "",               // MQTT password (lämna tomt)
  "klientnamn",          // Client name
  1883            // MQTT broker port
);
 
//Extern lampa
//#define led_pin D1


void setup() {
//pinMode(led_pin, OUTPUT);//Externlampa 
//digitalWrite(led_pin,LOW);
pinMode(LED_BUILTIN, OUTPUT); //Inbyggd lampa på kortet
digitalWrite(LED_BUILTIN,LOW);
Serial.begin(115200);
}

bool off=false;

void lampa(){
  if(off==true)
  {
  Serial.println("Släckt!");
  off=false;
  }
  else
  {
  off=true;
  
  Serial.println("Släckt!");
  }

//digitalWrite(led_pin,off);//Externlampa
digitalWrite(LED_BUILTIN,off);//Inbyggd lampa på kortet
  
}

void onConnectionEstablished()
{
  client.subscribe("abbjetmus/lampa", [] (const String &payload)
  {
    Serial.println(payload);
//    if(payload=="on")
  //  onlampa();
    lampa();
  });
  
  client.publish("abbjetmus/lampa", "This is a message");

  client.executeDelayed(5 * 1000, []() {
    client.publish("abbjetmus/lampa", "This is a message sent 5 seconds later");
  });
}


void loop() {
  // put your main code here, to run repeatedly:
client.loop();
}