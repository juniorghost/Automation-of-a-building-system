#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>

extern "C" {
#include <osapi.h>
#include <os_type.h>
}

#include "config.h"                     // Include Config Variables for Server Connection

static os_timer_t intervalTimer;   

static void replyToServer(void* arg) {                        //
	AsyncClient* client = reinterpret_cast<AsyncClient*>(arg);

	// send reply
	if (client->space() > 32 && client->canSend()) {                           //Checks to see if Client can send message
		char message[32];                                                        //Create Message Variable
		sprintf(message, "this is from %s", WiFi.localIP().toString().c_str());  // Create Message Text
		client->add(message, strlen(message));                                   // Add Message Text to Message 
		client->send();                                                          // Send Message
	}
}

////////////////// Client Events /////////////////////////////
static void handleData(void* arg, AsyncClient* client, void *data, size_t len) {              // Data Event Handling Function
	Serial.printf("\n data received from %s \n", client->remoteIP().toString().c_str());        // Create Message to be send.
	Serial.write((uint8_t*)data, len);                                                          // Write Message to Serial

	os_timer_arm(&intervalTimer, 2000, true);                                                   // Scheduel Reply Invetval at 2 seconds
}

void onConnect(void* arg, AsyncClient* client) {                                                  // Connection Event Handeling Function
	Serial.printf("\n client has been connected to %s on port %d \n", SERVER_HOST_NAME, TCP_PORT);  //Print Connection Information
	replyToServer(client);                                                                          // Send Client Info as Response to Server
}


void setup() {
	Serial.begin(115200);                        // Start Serial Monitor At 115200 baud
	delay(20);

	// connects to access point
	WiFi.mode(WIFI_STA);                         // Begin Wifi in STA Mod
	WiFi.begin(SSID, PASSWORD);                  // Connect to Wifi with SSID and PASSWORD from Config
	while (WiFi.status() != WL_CONNECTED) {      // While Waiting to connect
		Serial.print('.');                         // Print a Dot to Serial Monitor
		delay(500);
	}

	AsyncClient* client = new AsyncClient;       // Set up Async client as a New Client
	client->onData(&handleData, client);         // Set up Data Event Handleing
	client->onConnect(&onConnect, client);       // Set up Connection Event Handeling
	client->connect(SERVER_HOST_NAME, TCP_PORT); // On Connection Create Connection with Server with given hostname and port in Config File

	os_timer_disarm(&intervalTimer);
	os_timer_setfn(&intervalTimer, &replyToServer, client);
}

void loop() {

}
