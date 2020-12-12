/**********************************************************************************************************************************
  Arduino-Sketch für das NEOE-IOT-Kit-4, "Bodenfeuchtesensor mit NodeMCU. Arduino-Programmierung. MQTT-kompatibel zur Anbindung
  an Home Assistant."
  Dieser Arduino-Sketch wird in folgendem Tutorial verwendet:
  https://www.neoe.io/blogs/tutorials/bodenfeuchtesensor-mqtt-kompatibel-aufbau-variante-breadboard
  Die Programmierung wurde für folgendes Development-Board optimiert: NEOE WEMOS LOLIN32
  https://www.neoe.io/products/wemos-lolin32-v1-0-0-esp32-rev1-esp-wroom-32-wifi-bluetooth-board
  Fragen und Anregungen bitte in unserer Facebook-Gruppe adressieren, damit die gesamte Community davon profitiert.
  https://www.facebook.com/groups/neoe.io/
  Datum der letzten Änderung: 12. Dezember, 2020
**********************************************************************************************************************************/

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// WLAN-Zugangsdaten hier hinterlegen
const char* ssid = "NAME DES WLAN NETZWERKS"; // Anführungszeichen beibehalten
const char* password = "WLAN-PASSWORT"; // Anführungszeichen beibehalten, also z.B. so: "Geheim"

// Die für den MQTT-Server erforderlichen Daten hier hinterlegen
const char* mqtt_client = "NEOE-IOT-KIT-4-1"; // Wenn mehrere "NEOE IOT-Kits 4" im Einsatz sind, einfach mit der letzten Zahl durchnummerieren
const char* mqtt_server = "IP-ADRESSE DES MQTT-SERVERS"; // Anführungszeichen beibehalten, also z.B. so: "192.168.0.123"
const uint16_t mqtt_port = 1883;
const char* mqtt_user = "BENUTZERNAME"; // Anführungszeichen beibehalten
const char* mqtt_password = "PASSWORT"; // Anführungszeichen beibehalten, also z.B. so: "Geheim"

// MQTT-Topic für Home Assistant MQTT Auto Discovery
const char* mqtt_config_topic = "homeassistant/sensor/bodenfeuchte-farn-esszimmer/config"; // Name des Zimmers bei Bedarf ändern
const char* mqtt_state_topic = "homeassistant/sensor/bodenfeuchte-farn-esszimmer/state"; // Name des Zimmers bei Bedarf ändern

// Speicher-Reservierung für JSON-Dokument, kann mithilfe von arduinojson.org/v6/assistant noch optimiert werden
StaticJsonDocument<512> doc_config;
StaticJsonDocument<512> doc_state;

char mqtt_config_data[512];
char mqtt_state_data[512];

// Parameter für den Sensor
int sensePin = 34;
const int airValue = 3300; // Den aus der Kalibrierung gewonnennenen Wert für airValue hier hinterlegen
const int waterValue = 1650; // Den aus der Kalibrierung gewonnennenen Wert für waterValue hier hinterlegen
int soilMoistureValue = 0;
int soilMoisturePercent = 0;

// Parameter für die Indikator-LED
int LEDPIN = 4;

// Parameter für Sensor-Stromversorgung
int SENSORPOWERPIN = 2;

// Pausen zwischen Messungen definieren
int delay_time = 10000; // Alle 10 Sekunden wenn Bodenfeuchte < 50%

// esp_deep_sleep Parameter
#define uS_TO_S_FACTOR 1000000ULL  // Konvertierungsfaktor zur Umrechnung von Mikrosekunden in Sekunden
#define TIME_TO_SLEEP  3200  // 60 Minuten Deep Sleep, wenn Bodenfeuchte > 50%, um Strom zu sparen

WiFiClient espClient;
PubSubClient client(espClient);

// Funktion um Bodenfeuchte-Werte per MQTT zu übermitteln
void publishData(float p_bodenfeuchte) {
  doc_state["bodenfeuchte"] = round(p_bodenfeuchte);
  serializeJson(doc_state, mqtt_state_data);
  client.publish(mqtt_state_topic, mqtt_state_data);
}

// Funktion zur WLAN-Verbindung
void setup_wifi() {
  delay(10);
  /* Mit WLAN verbinden */
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

// Funktion zur MQTT-Verbindung
void reconnect() {
  while (!client.connected()) {
    if (client.connect(mqtt_client, mqtt_user, mqtt_password)) {
    } else {
      delay(5000);
    }
  }
}

// Funktion für Home Assistant MQTT Auto Discovery
void configMqtt() {
  doc_config["name"] = "Bodenfeuchte Farn Esszimmer";  // Name des Zimmers bei Bedarf ändern
  doc_config["state_topic"] = mqtt_state_topic;
  doc_config["unit_of_measurement"] = "%";
  doc_config["value_template"] = "{{ value_json.bodenfeuchte}}";
  serializeJson(doc_config, mqtt_config_data);
  client.publish(mqtt_config_topic, mqtt_config_data, true);
  delay(1000);
}

// Funktion für den DeepSleep Modus
void goToDeepSleep()
{
  // Timer bis zum Aufwachen konfigurieren
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  // DeepSleep starten
  esp_deep_sleep_start();
}

void setup() {
  pinMode(LEDPIN, OUTPUT);
  pinMode(SENSORPOWERPIN, OUTPUT);
  pinMode(sensePin, INPUT);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setBufferSize(512);
  if (!client.connected()) reconnect();
  configMqtt();
}

void loop() {

  if (!client.connected()) reconnect();
  digitalWrite(SENSORPOWERPIN, HIGH);
  delay(1000); // noch 2 Sekunden warten, damit der Sensor auch wirklich aktiv ist
  soilMoistureValue = analogRead(sensePin);
  soilMoisturePercent = map(soilMoistureValue, airValue, waterValue, 0, 100);
  digitalWrite(SENSORPOWERPIN, LOW);

  // Daten an den MQTT-Server senden
  publishData(soilMoisturePercent);

  // Wenn die Bodenfeuchte OK ist, Energiesparmodus für längeren Zeitraum aktivieren
  if (soilMoisturePercent > 50) {
    delay(2000); // noch 2 Sekunden warten, damit der letzte Messwert auch per MQTT übertragen werden kann
    goToDeepSleep();
  }

  // Bei trockener Erde oder falls Deep Sleep nicht funktioniert Indikator-LED blinken lassen
  digitalWrite(LEDPIN, HIGH);
  delay(50);
  digitalWrite(LEDPIN, LOW);

  // Pause zwischen Messungen
  delay(delay_time);

}
