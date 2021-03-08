/////////////////////////////////////////////
//                                         //
//         Authors: Salim El Atache        //
//             & Carson Morris             //
//         Class: CAM8302E - 012           //
//                 Lab: 5                  //
//           Date: 07-03-2021              //
//                                         //
/////////////////////////////////////////////

#include <Wire.h>              // I2C library
#include <Adafruit_SSD1306.h>  // OLED display library
#define OLED_ADDR  0x3C        // I2C address

Adafruit_SSD1306 display(-1);  // Set Display Reset Pin to the arduino reset pin.
                 
int pirPin = 5;               //PIR PIN Number

#include "DHT.h"              // DHT Library
#define DHTPIN 2              // DHT Digtal Pin Number (Connect to the second pin from the left on the Sensor)
#define DHTTYPE DHT22         // Set DHT type to DHT 22 (AM2302)
DHT dht(DHTPIN, DHTTYPE);     // Start DHT22 as object
float tempC;                  // Variable for holding current Room Temperature

#define fanRelay 3                 // Fan Relay Digital Pin Number
#define heatRelay 6                // Heat Relay Digital Pin Number

unsigned long delaytemp = 99999999999;   //Prevent Premature starting
unsigned long delaymotion = 99999999999;  //Prevent Premature starting

// Potentiometer Setup
const int analogInPin = A15;  // Pot Analog Pin Number
float sensorValue = 0;        // Current Set Temperature by Potentiometer
float MIN = 20;               // MIN value accessable by Thermostat 
float MAX = 30;               // MAX value accessable by Thermostat 
int SetPoint = 0;             // Current wanted SetPoint Temperature (Adjusted by Potentiometer)
int DeadBand = 2;             // Deadband Temperature Oscillation Varaible


void setup() {
  Serial.begin(9600);                               // Start Serial for Printing
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);   // Start OLED Screen and allow printing to specified address.

  dht.begin();                                      // Start DHT22 Object

  pinMode(fanRelay, OUTPUT);                        // Delclare fanRelay as an Output
  digitalWrite(fanRelay, LOW);                      // Start fanRelay OFF
  pinMode(heatRelay, OUTPUT);                       // Delclare heatRelay as an Output
  digitalWrite(heatRelay, LOW);                     // Start heatRelay OFF
}

void loop() {
  
  Motion();
  Display();            // Call function - Read Current Temperature and print to Display
  tempTrigger();        // Call function - Poteniometer to Set Temperature Setpoint
  //LIGHT FUNCTION ************************
  Fan_ON();             // Call function - Send Control Information to Other ESP
}

void Motion(){
  int pirVal = digitalRead(pirPin);                 // Read the state of the PIR Sensor
  if ((pirVal == LOW) ) {                           // Motion is detected 
  delaymotion = millis();                           // Set the flag value to TRUE
  }
}

void tempTrigger() {
  sensorValue = analogRead(analogInPin);             // Set sensor variable to current potentiometer reading
  SetPoint = map(sensorValue, 0, 1023, MIN, MAX);    // Map Setpoint of current sensorValue to a value between selected MIN and MAX values

  if (tempC >= (SetPoint + DeadBand / 2)) {          // Check if Temperature is between the desired setpoint with an allowable oscillation of the set deadband
    tempC = dht.readTemperature();                   // Read Current Temperature from Temp Sensor
    digitalWrite(heatRelay, LOW);                    // Turn heatRelay OFF

  }
    if (tempC <= (SetPoint - DeadBand / 2)) {        // Check if Temperature is between the desired setpoint with an allowable oscillation of the set deadband
    tempC = dht.readTemperature();                   // Read Current Temperature from Temp Sensor 
    delaytemp = millis();                            // Reset zero timer variable.
    digitalWrite(heatRelay, HIGH);                   // Turn heatRelay ON

  }
}

void Fan_ON() {
  if (((millis() - delaytemp) <= 3000 ) && ((millis() - delaymotion) <= 5000 ) ) {    // If it has been less than 15 seconds since timer variable has been zeroed 
    digitalWrite(fanRelay, HIGH);                // Turn Relay ON
  }
  else { 
    digitalWrite(fanRelay, LOW);                 // Turn Relay OFF
  }
}

void Display() {
  /////////////// DHT 22 and OLED display ////////////////////////
  tempC = dht.readTemperature();  // Read Current Temp in Degrees Celcius
  display.clearDisplay();         // Clear Display
  display.setTextColor(WHITE);    // Set Text Color
  display.setCursor(0, 0);        // Set the Position
  display.setTextSize(2);         // Set Text Size (2)
  display.println(tempC, 1);      // Print the Temp to Screen
  display.setCursor(52, 0);       // Set the Position
  display.print((char)247);       // Print the Degrees Symbol
  display.println("C");           // Print "C" to screen
  display.setCursor(90, 0);       // Set the Position
  display.setTextSize(1);         // Set Text Size (1)
  display.print("Set");           // Print "Set" to Screen
  display.setCursor(85, 7);       // Set the Position
  display.print("point");         // Print "point" to screen
  display.setTextSize(2);         // Set Text Size (2)
  display.setCursor(90, 16);      // Set the Position
  display.println(SetPoint);      // Write Current Setpoint to Display
  display.display();              // Update Display

}
