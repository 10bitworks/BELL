#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with your network details
#if 1
  const char* ssid = "10bitworks";
  const char* password = "10bitrocks";
  const char* mqtt_server = "10.7.1.81";
#endif


// Pin definition (e.g., GPIO 5 / D1 on NodeMCU)
const int relayPin = 5; 
const char* mqtt_topic = "bell";

WiFiClient espClient;
PubSubClient client(espClient);
uint32_t timer = 0;

void setup_wifi() {
  delay(10);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
}

// Function to handle incoming MQTT messages
void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (uint32_t i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.println(message);

  // Toggle the pin based on the message
  if (message == "1") {
    digitalWrite(relayPin, HIGH); // Turn pin ON
    Serial.println("Relay turned ON");
    timer = millis() + 100;
  } else if (message == "0") {
    digitalWrite(relayPin, LOW);  // Turn pin OFF
    Serial.println("Relay turned OFF");
    timer = 0;
  }
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP8266Client")) {
      // Subscribe to the topic when connected
      client.subscribe(mqtt_topic);
    } else {
      delay(5000);
     }
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("MQTT: Doorbell");
  Serial.println("Topic: bell. Payload: 0 = Off, 1 = On");
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW); // Start with pin OFF
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  if(millis() >= timer) {
    digitalWrite(relayPin, LOW);  // Turn pin OFF
  }
}