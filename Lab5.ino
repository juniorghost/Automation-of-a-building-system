/////////////////////////////////////////////
//                                         //
//         Authors: Salim El Atache        //
//             & Carson Morris             //
//         Class: CAM8302E - 012           //
//                 Lab: 5                  //
//           Date: 07-03-2021              //
//                                         //
/////////////////////////////////////////////

#include <Wire.h>                         // I2C library
#include "SSD1306Wire.h"                  // OLED display library          
SSD1306Wire display(0x3c, 4, 5);          // Set Display Address to talk on I2C Pins

int pirPin = 7;                           //PIR PIN Number

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
float sensorValue = 0;                    // Current Set Temperature by Potentiometer
float MIN = 20.0;                           // MIN value accessable by Thermostat 
float MAX = 30.0;                           // MAX value accessable by Thermostat 
float SetPoint = 0;                         // Current wanted SetPoint Temperature (Adjusted by Potentiometer)
int DeadBand = 2;                         // Deadband Temperature Oscillation Varaible

void setup() {
  Serial.begin(115200);                // Start Serial for Printing
  
  display.init();                      // Initialize OLED Display
  display.flipScreenVertically();      // Set Screen Orientation

  dht.begin();                         // Start DHT22 Object

//  pinMode(fanRelay, OUTPUT);         // Delclare fanRelay as an Output
//  digitalWrite(fanRelay, LOW);       // Start fanRelay OFF
//  pinMode(heatRelay, OUTPUT);        // Delclare heatRelay as an Output
//  digitalWrite(heatRelay, LOW);      // Start heatRelay OFF
}

void loop() {
  Motion();             // Call function - Read Current PIR Sensor for Motion
  Display();            // Call function - Read Current Temperature and print to Display
  tempTrigger();        // Call function - Poteniometer to Set Temperature Setpoint
  //LIGHT FUNCTION ************************
  Fan_ON();             // Call function - Send Control Information to Other ESP
}

void tempTrigger() {
  sensorValue = analogRead(analogInPin);             // Set sensor variable to current potentiometer reading
  SetPoint = map(sensorValue, 0, 1023, MIN, MAX);    // Map Setpoint of current sensorValue to a value between selected MIN and MAX values

  if (tempC >= (SetPoint + DeadBand / 2)) {          // Check if Temperature is between the desired setpoint with an allowable oscillation of the set deadband
    tempC = dht.readTemperature();                   // Read Current Temperature from Temp Sensor
//    digitalWrite(heatRelay, LOW);                    // Turn heatRelay OFF
  }
    if (tempC <= (SetPoint - DeadBand / 2)) {        // Check if Temperature is between the desired setpoint with an allowable oscillation of the set deadband
    tempC = dht.readTemperature();                   // Read Current Temperature from Temp Sensor 
    delaytemp = millis();                            // Reset zero timer variable.
//    digitalWrite(heatRelay, HIGH);                   // Turn heatRelay ON
  }
}

void Motion(){
  int pirVal = digitalRead(pirPin);      // Read the state of the PIR Sensor
  if ((pirVal == LOW) ) {                // Motion is detected 
  delaymotion = millis();                // Set the flag value to TRUE
  }
}

void Fan_ON() {
  if (((millis() - delaytemp) <= 3000 ) && ((millis() - delaymotion) <= 5000 ) ) {  
//    digitalWrite(fanRelay, HIGH);                // Turn Relay ON
  }
  else { 
//    digitalWrite(fanRelay, LOW);                 // Turn Relay OFF
  }
}

void Display() {
  /////////////// DHT 22 and OLED display ////////////////////////
  tempC = dht.readTemperature();     // Read Current Temp in Degrees Celcius
  display.clear();                   // Clear Display
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_24); // Adjust Font Size
  display.drawString(0, 0, String(tempC));         // Print the Temp to Screen
  display.drawString(59, 0, String((char)247));          // Print the Degrees Symbol
  display.drawString(63, 0, "C");           // Print "C" to screen
  display.setFont(ArialMT_Plain_10); // Adjust Font Size
  display.drawString(90, 0, "Set");         // Print "Set" to Screen
  display.drawString(86, 7, "point");       // Print "point" to screen
  display.setFont(ArialMT_Plain_16); // Adjust Font Size
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(98, 18, String(SetPoint));       // Write Current Setpoint to Display
  display.display();                    // Update Display
}
