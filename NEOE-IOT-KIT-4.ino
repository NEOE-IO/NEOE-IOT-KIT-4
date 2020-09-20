/**********************************************************************************************************************************
  Arduino-Sketch für das NEOE IOT-Kit 4, "Bodenfeuchtesensor mit NodeMCU. Für Arduino, MQTT, Node-Red." 
  https://www.neoe.io/products/bausatz-bodenfeuchtesensor-mit-nodemcu-fur-arduino-mqtt-node-red
  Fragen und Anregungen bitte in unserer Facebook-Gruppe adressieren, damit die gesamte Community davon profitiert. 
  https://www.facebook.com/groups/neoe.io/
  Datum der letzten Änderung: 20. September, 2020
**********************************************************************************************************************************/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

/* WLAN-Zugangsdaten hier hinterlegen */ 
const char* ssid = "NAME DES WLAN NETZWERKS"; // Anführungszeichen beibehalten
const char* password = "WLAN-PASSWORT"; // Anführungszeichen beibehalten, also z.B. so: "Geheim"

/* Die für den MQTT-Server erforderlichen Daten hier hinterlegen */ 
const char* mqttServer = "IP-ADRESSE DES MQTT-SERVERS"; // Anführungszeichen beibehalten, also z.B. so: "192.168.0.236"
const uint16_t mqttPort = 1883; // Bei Bedarf den Port des MQTT-Servers hier ändern
const char* clientId = "NEOE-IOT-KIT-4-1"; // Wenn mehrere "NEOE IOT-Kits 4" im Einsatz sind, einfach mit der letzten Zahl durchnummerieren

/* Parameter für den Sensor */
int sensePin = 0; 
const int airValue = 880; // Den aus der Kalibrierung gewonnennenen Wert für airValue hier hinterlegen
const int waterValue = 474; // Den aus der Kalibrierung gewonnennenen Wert für airValue hier hinterlegen
int soilMoistureValue = 0;
int soilMoisturePercent = 0;

/* Parameter für die Indikator-LED */
int LEDPIN = 4;
float LEDValue = 0;

int counter;

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup_wifi() {
  delay(10);
  /* Mit WLAN verbinden */
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void reconnect() {
  /* Drei Verbindungsversuche, dann geht es im Offlinebetrieb weiter */
  counter = 1;
  while (!client.connected() && counter<=3) {
    if (client.connect(clientId)) {
      client.subscribe("inTopic");
      counter++;
    } 
  }
}

void setup() {
  
  setup_wifi();
  
  client.setServer(mqttServer, mqttPort);
  
}

void loop() {
  
  if (!client.connected()) reconnect();

  soilMoistureValue = analogRead(sensePin);
  soilMoisturePercent = map(soilMoistureValue, airValue, waterValue, 0, 100);

  /* Daten an den MQTT-Server senden */
  client.publish("outSoilMoisture01", String(soilMoisturePercent).c_str());

  /* Bei zu trockener Erde blinkt die LED */
  if (soilMoisturePercent < 50) {
    LEDValue = 1024;
    analogWrite(LEDPIN, LEDValue);
    delay(50);
    LEDValue = 0;
    analogWrite(LEDPIN, LEDValue);
  }

  delay(5000);

  /* Wenn die Bodenfeuchte OK ist, Energiesparmodus für 10 Minuten aktivieren */
  if (soilMoisturePercent > 50) ESP.deepSleep(10 * 60 * 1000000);

}
