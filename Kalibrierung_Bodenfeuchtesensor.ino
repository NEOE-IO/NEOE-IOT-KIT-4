/**********************************************************************************************************************************
  Arduino-Sketch für die Kalibrierung das NEOE IOT-Kit 4, "Bodenfeuchtesensor mit NodeMCU. Arduino-Programmierung. 
  MQTT-kompatibel zur Anbindung an Home Assistant."
  Dieser Arduino-Sketch wird in folgendem Tutorial verwendet:
  https://www.neoe.io/blogs/tutorials/bodenfeuchtesensor-mqtt-kompatibel-aufbau-variante-breadboard
  Fragen und Anregungen bitte in unserer Facebook-Gruppe adressieren, damit die gesamte Community davon profitiert. 
  https://www.facebook.com/groups/neoe.io/
  Datum der letzten Änderung: 12. Dezember, 2020
**********************************************************************************************************************************/

// Parameter für den Sensor
int sensePin = 34;

// Parameter für Sensor-Stromversorgung
int SENSORPOWERPIN = 2;

int soilMoistureValue = 0;

void setup() {
  pinMode(SENSORPOWERPIN, OUTPUT);
  pinMode(sensePin, INPUT); 
  /* Der Kalibrierungswert wird am Seriellen Monitor ausgegeben. 
  Im Seriellen Monitor die Baudrate bitte auf den definierten Wert von 9600 einstellen.
  Nur so können die Daten im Seriellen Monitro angezeigt werden können*/
  Serial.begin(9600);
  digitalWrite(SENSORPOWERPIN, HIGH);
  delay(1000);
}

void loop() {
  soilMoistureValue = analogRead(sensePin);
  Serial.println(soilMoistureValue); 
  delay(1000);
}
