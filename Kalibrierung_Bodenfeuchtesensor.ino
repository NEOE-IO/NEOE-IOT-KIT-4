/**********************************************************************************************************************************
  Arduino-Sketch für die Kalibrierung das NEOE IOT-Kit 4, "Bodenfeuchtesensor mit NodeMCU. Für Arduino, MQTT, Node-Red." 
  https://www.neoe.io/products/bausatz-bodenfeuchtesensor-mit-nodemcu-fur-arduino-mqtt-node-red
  Fragen und Anregungen bitte in unserer Facebook-Gruppe adressieren, damit die gesamte Community davon profitiert. 
  https://www.facebook.com/groups/neoe.io/
  Datum der letzten Änderung: 20. September, 2020
**********************************************************************************************************************************/

/* Parameter für den Sensor */
int sensePin = 0; 
int soilMoistureValue = 0;

void setup() {
  /* Der Kalibrierungswert wird am Seriellen Monitor ausgegeben. 
  Im Seriellen Monitor die Baudrate bitte auf den definierten Wert von 9600 einstellen.
  Nur so können die Daten im Seriellen Monitro angezeigt werden können*/
  Serial.begin(9600);
}

void loop() {
  soilMoistureValue = analogRead(sensePin);
  Serial.println(soilMoistureValue); 
  delay(1000);
}
