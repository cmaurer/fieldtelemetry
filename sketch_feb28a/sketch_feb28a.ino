/*  
  GPRS
  Temperature
  Moisture

  Temperature: Pin 10
  Moisture: Pin 0
  GPRS: Pins 7 and 8

*/
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SoftwareSerial.h>
#include<WString.h>

#define ONEWIRE 10 //pin to use for One Wire interface
#define MOISTURE 0 //pin to use for Moisture Sensor
#define DELAY 5000

OneWire oneWire(ONEWIRE);
DallasTemperature sensors(&oneWire);

//-------------------------------------------------------------
//---------------------Ubidots Configuration-------------------
//-------------------------------------------------------------
String token = "";      
String idvariable = "";                                  

void setup() {
  //the GPRS baud rate
  Serial1.begin(19200);                                                             
  Serial.begin(9600);
  // Start up the OneWire Sensors library
  sensors.begin();
  delay(1000);
  Serial.println(ONEWIRE);
}

void loop() {
  String var;
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
  float moistureVoltage = moistureValue * (5.0 / 1023.0);
  Serial.print("voltage: ");
  Serial.println(moistureVoltage);
  
  var = "{\"fahrenheit\":"+ String(tempF) + ",\"moisture\":" + String(moistureValue) + ",\"voltage\":" + String(moistureVoltage) + "}";  
  Serial.println(var);
 // upload(var);

  //Ubidots
  delay(DELAY);
}

void upload(String value){
  int num;
  String le;
  String var;
  var="{\"value\":"+ value + "}";
  num=var.length();
  le=String(num);
  for(int i = 0; i < 7; i++){    
    //this is made repeatedly because it is unstable    
    Serial1.println("AT+CGATT?");
    delay(2000);
    ShowSerialData();
  }
  
  //replace with your providers' APN
  Serial1.println("AT+CSTT=\"att.mvno\"");                                    
  delay(1000);
  ShowSerialData();
  //bring up wireless connection
  Serial1.println("AT+CIICR");
  delay(3000);
  ShowSerialData();
  //get local IP adress
  Serial1.println("AT+CIFSR");
  delay(2000);
  ShowSerialData();
  Serial1.println("AT+CIPSPRT=0");
  delay(3000);
  ShowSerialData();
  //start up the connection
  Serial1.println("AT+CIPSTART=\"tcp\",\"things.ubidots.com\",\"80\"");
  delay(3000);
  ShowSerialData();
  //begin send data to remote server
  Serial1.println("AT+CIPSEND");
  delay(3000);
  ShowSerialData();
  Serial1.print("POST /api/v1.6/variables/" + idvariable);
  delay(100);
  ShowSerialData();
  Serial1.println("/values HTTP/1.1");
  delay(100);
  ShowSerialData();
  Serial1.println("Content-Type: application/json");
  delay(100);
  ShowSerialData();
  Serial1.println("Content-Length: "+le);
  delay(100);
  ShowSerialData();
  Serial1.print("X-Auth-Token: ");
  delay(100);
  ShowSerialData();
  Serial1.println(token);
  delay(100);
  ShowSerialData();
  Serial1.println("Host: things.ubidots.com");
  delay(100);
  ShowSerialData();
  Serial1.println();
  delay(100);
  ShowSerialData();
  Serial1.println(var);
  delay(100);
  ShowSerialData();
  Serial1.println();
  delay(100);
  ShowSerialData();
  Serial1.println((char)26);
  delay(7000);
  Serial1.println();
  ShowSerialData();
  //close the communication
  Serial1.println("AT+CIPCLOSE");
  delay(1000);
  ShowSerialData();
}

void ShowSerialData() {
  while(Serial1.available()!=0)
  Serial.write(Serial1.read());
}

