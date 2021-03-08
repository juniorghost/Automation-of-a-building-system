#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <DNSServer.h>
#include <vector>

#include "config.h"

static DNSServer DNS;
static std::vector<AsyncClient*> clients;  // A List to Hold all Clients

/////////////////////////////////////////// CONNECTION SERVER ///////////////////////////////////////////////

//////////////////  Client Events /////////////////////////////
static void handleError(void* arg, AsyncClient* client, int8_t error) {                  //If Statement for Client ERROR Handeling
	Serial.printf("\n Connection Error %s from client %s \n", client->errorToString(error), client->remoteIP().toString().c_str()); //Print Client Error to Serial
}

static void handleData(void* arg, AsyncClient* client, void *data, size_t len) {     //If Statement for Client DATA Handeling
	Serial.printf(client->remoteIP().toString().c_str());                             //Print Client Data Recieved to Terminal
	Serial.write((uint8_t*)data, len);

	// Reply to Client
	if (client->space() > 32 && client->canSend()) {         // Check to See if Client is avaliable
		char reply[32];                                        // Create Reply Variable
		sprintf(reply, "this is from %s", SERVER_HOST_NAME);   // Create Replay Message                        
		client->add(reply, strlen(reply));                     // Add message to reply 
		client->send();                                        // Send Message to Client
	}
}

static void handleDisconnect(void* arg, AsyncClient* client) {                              // If Statement for Client DISCONNECT Handeling   
	Serial.printf("\n client %s disconnected \n", client->remoteIP().toString().c_str());     // Print Disconnect IP to Screen
}

static void handleTimeOut(void* arg, AsyncClient* client, uint32_t time) {                  // If Statement for Client TIMEOUT Handeling    
	Serial.printf("\n client ACK timeout ip: %s \n", client->remoteIP().toString().c_str());  // Write Timeout IP to Screen
}


//////////////////  Server Events /////////////////////////////
static void handleNewClient(void* arg, AsyncClient* client) {                                                    // If Statement for new Client CONNECTION Handeling 
	Serial.printf("\n new client has been connected to server, ip: %s", client->remoteIP().toString().c_str());   // Client Connection IP to Screen

	clients.push_back(client);                          // Add Client to Generated List
	
	// Register Events
	client->onData(&handleData, NULL);
	client->onError(&handleError, NULL);
	client->onDisconnect(&handleDisconnect, NULL);
	client->onTimeout(&handleTimeOut, NULL);
}

void setup() {
	Serial.begin(115200);
	delay(20);
	
	// Create Access Point
	while (!WiFi.softAP(SSID, PASSWORD, 6, false, 15)) {
		delay(500);
	}

	// Start DNS Server
	if (!DNS.start(DNS_PORT, SERVER_HOST_NAME, WiFi.softAPIP()))  //If Server Failed to Start
		Serial.printf("\n failed to start dns service \n");         //Print Fail Message

	AsyncServer* server = new AsyncServer(TCP_PORT);             // Start Listening on TCP Port 7050
	server->onClient(&handleNewClient, server);                  // Set up Client Event Handeling
	server->begin();                                             // Start Server communication
}

void loop() {
	DNS.processNextRequest();                                    // Procss DNS Requests
}
