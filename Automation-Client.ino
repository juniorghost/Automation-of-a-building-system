//                                         //
//         Authors: Salim El Atache        //
//             & Carson Morris             //
//              CAM8302E - 012             //
//                                         //
//           Date: 07-03-2021              //
//                                         //
/////////////////////////////////////////////

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti WiFiMulti;

// Wifi Connection Variables
const char* ssid = "ControlSystem";
const char* password = "12345678";

//Server Request Functions//
const char* serverFanON = "http://192.168.4.1/fanON";
const char* serverFanOFF = "http://192.168.4.1/fanOFF";
const char* serverHeaterON = "http://192.168.4.1/heaterON";
const char* serverHeaterOFF = "http://192.168.4.1/heaterOFF";
const char* serverLockON = "http://192.168.4.1/lockON";
const char* serverLockOFF = "http://192.168.4.1/lockOFF";
const char* serverNameLight = "http://192.168.4.1/light";

#include <Wire.h>                         // I2C library
#include "SSD1306Wire.h"                  // OLED display library          
SSD1306Wire display(0x3c, 4, 5);          // Set Display Address to talk on I2C Pins

int pirVal;
int pirPin = 12;                           //PIR PIN Number

#include "DHT.h"                          // DHT Library
#define DHTPIN 2                          // DHT Digtal Pin Number (Connect to the second pin from the left on the Sensor)
#define DHTTYPE DHT22                     // Set DHT type to DHT 22 (AM2302)
DHT dht(DHTPIN, DHTTYPE);                 // Start DHT22 as object
float tempC;                              // Variable for holding current Room Temperature

//#define fanRelay 5                        // Fan Relay Digital Pin Number
//#define heatRelay 6                       // Heat Relay Digital Pin Number

unsigned long delaytemp = 99999999999;    //Prevent Premature starting
unsigned long delaymotion = 99999999999;  //Prevent Premature starting

// Potentiometer Setup                    
const int analogInPin = A0;              // Pot Analog Pin Number
float sensorValue = 0;                   // Current Set Temperature by Potentiometer
float MIN = 20.0;                        // MIN value accessable by Thermostat 
float MAX = 30.0;                        // MAX value accessable by Thermostat 
float SetPoint = 0;                      // Current wanted SetPoint Temperature (Adjusted by Potentiometer)
int DeadBand = 2;                        // Deadband Temperature Oscillation Varaible

void setup() {
  Serial.begin(115200);                // Start Serial for Printing

  display.init();                      // Initialize OLED Display
  display.flipScreenVertically();      // Set Screen Orientation

  dht.begin();                         // Start DHT22 Object
  WiFi.mode(WIFI_STA);                        // Set up Wifi Client Communication
  WiFiMulti.addAP(ssid, password);            // Connect to Network Using Set Username and Password
  while((WiFiMulti.run() == WL_CONNECTED)) {  // While waiting to connect
    delay(500);
    Serial.print(".");                        //Print point to show pending
  }
  Serial.println("");
  Serial.println("Connected to WiFi");       // Print Connected when finished while loop
}

void loop() {
  Serial.print(pirVal);
  if ((WiFiMulti.run() == WL_CONNECTED)) {  // If there is a currently active connection
    Motion();             // Call function - Read Current PIR Sensor for Motion
    Display();            // Call function - Read Current Temperature and print to Display
    tempTrigger();        // Call function - Poteniometer to Set Temperature Setpoint
    //LIGHT FUNCTION ************************
    Fan_ON();             // Call function - Send Control Information to Other ESP
  } else {
      Serial.println("WiFi Disconnected");
  }
}

void tempTrigger() {
  sensorValue = analogRead(analogInPin);             // Set sensor variable to current potentiometer reading
  SetPoint = map(sensorValue, 0, 1023, MIN, MAX);    // Map Setpoint of current sensorValue to a value between selected MIN and MAX values

  if (tempC >= (SetPoint + DeadBand / 2)) {          // Check if Temperature is between the desired setpoint with an allowable oscillation of the set deadband
    tempC = dht.readTemperature();                   // Read Current Temperature from Temp Sensor
      httpGETRequest(serverHeaterOFF);        //Send Request to Server to turn ON Fan
  }
    if (tempC <= (SetPoint - DeadBand / 2)) {        // Check if Temperature is between the desired setpoint with an allowable oscillation of the set deadband
    tempC = dht.readTemperature();                   // Read Current Temperature from Temp Sensor 
    delaytemp = millis();                            // Reset zero timer variable.
    httpGETRequest(serverHeaterON);        //Send Request to Server to turn ON Fan
  }
}

void Motion(){
  pirVal = digitalRead(pirPin);      // Read the state of the PIR Sensor
  if ((pirVal == HIGH) ) {                // Motion is detected 
  delaymotion = millis();                // Set the flag value to TRUE
  }
}

void Fan_ON() {
  if (((millis() - delaytemp) <= 3000 ) && ((millis() - delaymotion) <= 5000 ) ) {  
        httpGETRequest(serverFanON);        //Send Request to Server to turn ON Fan
  }
  else { 
        httpGETRequest(serverFanOFF);        //Send Request to Server to turn ON Fan
  }
}

void Display() {
  /////////////// DHT 22 and OLED display ////////////////////////
  tempC = dht.readTemperature();                   // Read Current Temp in Degrees Celcius
  display.clear();                                 // Clear Display
  display.setTextAlignment(TEXT_ALIGN_LEFT);       // Set Text Allignment
  display.setFont(ArialMT_Plain_24);               // Adjust Font Size
  display.drawString(0, 0, String(tempC));         // Print the Temp to Screen
  display.drawString(59, 0, String((char)247));    // Print the Degrees Symbol
  display.drawString(63, 0, "C");                  // Print "C" to screen
  display.setFont(ArialMT_Plain_10);               // Adjust Font Size
  display.drawString(90, 0, "Set");                // Print "Set" to Screen
  display.drawString(86, 7, "point");              // Print "point" to screen
  display.setFont(ArialMT_Plain_16);               // Adjust Font Size
  display.setTextAlignment(TEXT_ALIGN_CENTER);     // Set Text Allignment
  display.drawString(98, 18, String(SetPoint));    // Write Current Setpoint to Display
  display.display();                               // Update Display
}

String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;
    
  http.begin(client, serverName);     // Your IP address with path or Domain name with URL path 
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "--"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }

  http.end();

  return payload;
}

/////  COMMANDS  /////////
/*
 * 
 *       httpGETRequest(serverFanON);        //Send Request to Server to turn ON Fan
      //httpGETRequest(serverFanOFF);     //Send Request to Server to turn OFF Fan
      //httpGETRequest(serverHeaterON);   //Send Request to Server to turn ON Heater
      //httpGETRequest(serverHeaterOFF);  //Send Request to Server to turn OFF Heater
      //httpGETRequest(serverLockON);     //Send Request to Server to turn ON Lock
      //httpGETRequest(serverLockOFF);    //Send Request to Server to turn OFF Lock
      //httpGETRequest(serverNameLight);
      // A Response of 200 Means Command Accepted //
    }

*/
