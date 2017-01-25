//  Copyright (C) 2017 James Mutton
//  http://themuttons.com
//
//  This file is part of the PCoT project (Private Cloud of Thing)
//
//  PCoT and all its components are licensesd LGPL 3+
//
//  MotionSensor is free software: you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public License
//  as published by the Free Software Foundation, either version 3 of
//  the License, or (at your option) any later version.
//
//  MotionSensor is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with MotionSensor. If not, see
//  <http://www.gnu.org/licenses/>.

#include <SPI.h>
#include <Ethernet.h>
// from https://github.com/jamutton/pubsubclient 
#include <PubSubClient.h>
// from https://github.com/jamutton/EthernetBonjour
#include <EthernetBonjour.h>

// the media access control (ethernet hardware) address for the shield.
// Make this up/modify for each shield.
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };  

// The PCoT thingdentifier
const char* MyPCoTID = "sens-mo-12345";
const char* MyPCoTRouter = "events1";

// announcement protocol message frequency in mills
int announceMills = 25000;

// for mdns discovery (or .h)
const char* ip_to_str(const uint8_t*);
void nameFound(const char* name, const byte ipAddr[4]);

// using one or the other, bonjour or direct IP.
//const char* mqtt_server = "events1.local";
IPAddress mqtt_server(10, 10, 10, 126);

EthernetClient ethClient;
PubSubClient client(ethClient);

long lastMsg = 0, lastHelloMsg = 0;
int value = 0;
uint8_t* rtrIPAddr;
char resolved = 0;
long sensorTriggerMills = 10000; // send motion message only once every THIS mills

void etherUp() {
  int ready = 0;
  delay(10);
  // We start by connecting to a network
  Serial.println();
  Serial.print("Connecting to network... ");

  while ( ! ready ) {
    ready = Ethernet.begin(mac);
    if ( ready ) {
      Serial.println("successful.");
    } else {
      Serial.println("failed.");
      delay(5000); // ready will be non1 so we'll try again in 5 seconds
    }
  }
  Serial.println("");
  Serial.print("IP address: ");
  Serial.println(Ethernet.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "arduinoClient-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}



/**
 * BEGIN : Specific functions for this sensor
 */
int pirInputPin = 2;            // choose the input pin for PIR sensor
int pirState = LOW;             // we start, assuming no motion detected

void setup()
{
  Serial.begin(115200);
  //PCoT_Setup(); // This should abstract all the ethernet/wifi and MQTT setup
  //PCoT begin
  //setup network  
  etherUp();
  Serial.print("Resolving PCoT Event router '");
  Serial.print(MyPCoTRouter);
  Serial.println("' via Multi-Cast DNS (Bonjour)...");
  // signal discovery of our events router
  // register mDNS for name resolution
  // in the case of PCoT, all messages are routed through a MQTT host so service-disco isn't required.
  EthernetBonjour.begin(MyPCoTID);
  EthernetBonjour.setNameResolvedCallback(nameFound);
  EthernetBonjour.resolveName(MyPCoTRouter, 0);
  //PCoT end

  // TYPICAL SETUP WOULD BEGIN HERE:
  pinMode(pirInputPin, INPUT);  // declare sensor input
}

void loop() {
  char msg[50];
  int val = 0;                    // variable for reading the pin status

  if ( !resolved ) {
    // we must still be waiting for resolution, just cycle
    EthernetBonjour.run();
  } else {  // normal loop
    // handle reconnects and send hello-protocol messages
    if (!client.connected()) {
      reconnect();
    }
    client.loop();
    long now = millis();
    if (now - lastHelloMsg > announceMills) {
      lastHelloMsg = now;
      ++value;
      snprintf (msg, 50, "hello_%s_%ld", MyPCoTID, value);
      Serial.print("Publish message: ");
      Serial.println(msg);
      client.publish("announce/sensor/pir", msg);
    }
    val = digitalRead(pirInputPin);  // read input value
    if (val == HIGH) {            // check if the input is HIGH
      if (now - lastMsg > sensorTriggerMills) {
        lastMsg = now;
        Serial.println("Motion detected!");
        client.publish("sensors/motion/LOCATION", "MOTION!");
      }
    }
  } // end normal loop
}

/**
 * Sections below for mDNS Discovery
 */
// This function is called when a name is resolved via MDNS/Bonjour. We set
// this up in the setup() function above. The name you give to this callback
// function does not matter at all, but it must take exactly these arguments
// (a const char*, which is the hostName you wanted resolved, and a const
// byte[4], which contains the IP address of the host on success, or NULL if
// the name resolution timed out).
void nameFound(const char* name, const byte ipAddr[4])
{
  if (NULL != ipAddr) {
    Serial.print("The IP address for '");
    Serial.print(name);
    Serial.print("' is ");
    Serial.println(ip_to_str(ipAddr));
    Serial.println("Connecting to MQTT Server");
    client.setServer(ipAddr, 1883);
    resolved = 1;
  } else {
    Serial.print("Resolving '");
    Serial.print(name);
    Serial.println("' timed out.  Restart to continue.");
  }
}

// This is just a little utility function to format an IP address as a string.
const char* ip_to_str(const uint8_t* ipAddr) {
  static char buf[16];
  sprintf(buf, "%d.%d.%d.%d\0", ipAddr[0], ipAddr[1], ipAddr[2], ipAddr[3]);
  return buf;
}
