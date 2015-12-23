#include <EEPROM.h>

/*
  GPRS
  Temperature
  Moisture

  Temperature: Pin 10
  Moisture: Pin 0
  GPRS: Pins 18 and 19
  Photo Cell: Pin 8

*/
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SoftwareSerial.h>
#include<WString.h>

#define ONEWIRE 10 //pin to use for One Wire interface
#define MOISTURE 0 //pin to use for Moisture Sensor
#define PHOTO 8 //pin to use for Photocell Sensor
#define AMBIENT 9 //pin to use for Ambient Light Sensor

#define DELAY 60000 //delay each measurement for 60 seconds

#define SEMATEXT_TOKEN String("token")

OneWire oneWire(ONEWIRE);
DallasTemperature sensors(&oneWire);

void setup() {
  //the GPRS baud rate
  Serial1.begin(19200);
  Serial.begin(19200);

  // Start up the OneWire Sensors library
  sensors.begin();
  delay(1000);

}

void loop() {

  //Temperature
  sensors.requestTemperatures(); // Send the command to get temperatures
  float tempC = sensors.getTempCByIndex(0);
  float tempF = DallasTemperature::toFahrenheit(tempC);
  Serial.print("Fahrenheit: ");
  Serial.print(tempF);
  Serial.println(" F ..........DONE");

  //Moisture
  int moistureValue = analogRead(MOISTURE);
  Serial.print("moistureValue: ");
  Serial.println(moistureValue);

  //Moisture Voltage
  float moistureVoltage = moistureValue * (5.0 / 1023.0);
  Serial.print("voltage: ");
  Serial.println(moistureVoltage);

  int lightLevel = analogRead(PHOTO);
  Serial.print("light: ");
  Serial.println(lightLevel);

  int ambientLightLevel = analogRead(AMBIENT);
  Serial.print("ambient light: ");
  Serial.println(ambientLightLevel);

  //send the data
  upload(tempC, tempF, moistureValue, moistureVoltage, lightLevel, ambientLightLevel);

  delay(DELAY);
}

void upload(float tempc, float tempf, int moistureValue, float moistureVoltage, int lightLevel, int ambientLightLevel) {

    String payload = convertToJsonString(tempc, tempf, moistureValue, moistureVoltage, lightLevel, ambientLightLevel);
    int num = payload.length();

    sendATCommand("AT+CIPCLOSE", 100);
    sendATCommand("AT+CIPSHUT", 100);

    sendATCommand("AT+CPIN?", 1000);
    sendATCommand("AT+CSQ", 1000);
    sendATCommand("AT+CREG?", 1000);
    sendATCommand("AT+CGATT?", 500);

    sendATCommand("AT+CGATT=1", 500);

    sendATCommand("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"", 500);
    sendATCommand("AT+SAPBR=3,1,\"APN\",\"wholesale\"", 500);
    sendATCommand("AT+SAPBR=1,1", 3000);

    sendATCommand("AT+HTTPINIT", 3000);
    sendATCommand("AT+HTTPPARA=\"CID\",1", 500);
    sendATCommand("AT+HTTPPARA=\"URL\",\"logsene-receiver.sematext.com/"+SEMATEXT_TOKEN+"/field-telemetry\"", 500);
    sendATCommand("AT+HTTPPARA=\"CONTENT\",\"application/json\"", 500);
    sendATCommand("AT+HTTPPARA=\"HOST\",\"https://logsene-receiver.sematext.com\"", 500);

    sendATCommand("AT+HTTPDATA=" + String(payload.length()) + ",5000", 500);
    writeData(payload, 1000);
    sendATCommand("AT+HTTPACTION=1", 5000);
    sendATCommand("AT+HTTPREAD", 500);
    sendATCommand("AT+HTTPTERM", 500);
    sendATCommand("AT+SAPBR=0,1", 500);

}

String convertToJsonString(float tempc, float tempf, int moistureValue, float moistureVoltage, int lightLevel, int ambientLightLevel) {
 return "{ \"temperature_celcius\": " +  String(tempc) + ", \"temperature_fahrenheit\": " + String(tempf) + ", \"moisture_value\":" + String(moistureValue) + ",\"moisture_voltage\": " + String(moistureVoltage) + ", \"light_level\":" + String(lightLevel)+ ",\"ambient_light_level\":" + String(ambientLightLevel) + " }";
}

void writeData(String data, int _delay) {
  Serial1.print(data);
  delay(_delay);
  ShowSerialData();
}

void writeData(char data, int _delay) {
  Serial1.print(data);
  delay(_delay);
  ShowSerialData();
}

void sendATCommand(String data, int _delay) {
  Serial1.println(data);
  delay(_delay);
  ShowSerialData();
}

void ShowSerialData() {
  while(Serial1.available() != 0){
   Serial.write(Serial1.read());
  }
}
