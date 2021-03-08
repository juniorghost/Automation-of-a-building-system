#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti WiFiMulti;

const char* ssid = "ESP8266-Access-Point";
const char* password = "123456789";

//Your IP address or domain name with URL path
const char* serverNameFan = "http://192.168.4.1/fan";
const char* serverNameHeater = "http://192.168.4.1/heater";
const char* serverNameLock = "http://192.168.4.1/lock";
const char* serverNameLight = "http://192.168.4.1/light";

String fan;
String heater;
String lock;
String light;


unsigned long previousMillis = 0;
const long interval = 5000; 

void setup() {
  Serial.begin(115200);
  Serial.println();
  
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
    // Check WiFi connection status
    if ((WiFiMulti.run() == WL_CONNECTED)) {
      fan = httpGETRequest(serverNameFan);
      Serial.println(fan);
      //heater = httpGETRequest(serverNameHeater);
      //lock = httpGETRequest(serverNameLock);
      //light = httpGETRequest(serverNameLight);
    }
    else {
      Serial.println("WiFi Disconnected");
    }
}


String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;
    
  // Your IP address with path or Domain name with URL path 
  http.begin(client, serverName);
  
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
  // Free resources
  http.end();

  return payload;
}
