// Import required libraries
#include <ESP8266WiFi.h>
#include "ESPAsyncWebServer.h"

// Set your access point network credentials
const char* ssid = "ESP8266-Access-Point";
const char* password = "123456789";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  Serial.println();
  
  // Set up Access Point
  Serial.print("Setting AP (Access Point)…");
  WiFi.softAP(ssid, password); //Set up Access Point with Set SSID and Password

  IPAddress IP = WiFi.softAPIP(); // Set the IP address by newly created Access Point
  Serial.print("\nAP IP address: ");
  Serial.println(IP);             // Print New IP address to Serial Monitor

  server.on("/fan", HTTP_GET, [](AsyncWebServerRequest *request){     // When and HTTP Get Request is made to (IP)/temperature 
    /////////RUN FUNCTION HERE////////////
    request->send_P(200, "text/plain", "Is it working");                  // Return Message to Client With        
  });
  server.on("/heater", HTTP_GET, [](AsyncWebServerRequest *request){       // When and HTTP Get Request is made to (IP)/humidity
    /////////RUN FUNCTION HERE////////////
    request->send_P(200, "text/plain", "Heater ON");                  // Return Message to Client
  });
    server.on("/lock", HTTP_GET, [](AsyncWebServerRequest *request){     // When and HTTP Get Request is made to (IP)/temperature 
    /////////RUN FUNCTION HERE////////////
    request->send_P(200, "text/plain", "Lock ON");                  // Return Message to Client With        
  });
  server.on("/light", HTTP_GET, [](AsyncWebServerRequest *request){       // When and HTTP Get Request is made to (IP)/humidity
    /////////RUN FUNCTION HERE////////////
    request->send_P(200, "text/plain", "Light ON");                  // Return Message to Client
  });
  
  bool status;
 
  server.begin();   // Start Server
}
 
void loop(){
  
}
