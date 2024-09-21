#include <WiFi.h>
#include <PubSubClient.h>
#include "MQTTConnectionManager.h"

WiFiClient espClient;                 // initialize wifi client
PubSubClient mqtt_client(espClient);  // and the mqtt client, which will use the wifi

MQTTConnectionManager::MQTTConnectionManager(
  const char* mqtt_broker,         // server to use
  const unsigned short mqtt_port,  // port
  const unsigned long cooldown,    // time between connection attempts
  const char* mqtt_username,       // username
  const char* mqtt_password        // password
) {
  _mqtt_broker = mqtt_broker,
  _mqtt_port = mqtt_port;
  _cooldown = cooldown;
  _mqtt_username = mqtt_username;
  _mqtt_password = mqtt_password;
}

void MQTTConnectionManager::begin(void (*callback)(char*, byte*, unsigned int)) {
  mqtt_client.setServer(_mqtt_broker, _mqtt_port);  // set the server and port to use
  Serial.print("Server set to: ");
  Serial.print(_mqtt_broker);
  Serial.print(" port: ");
  Serial.println(_mqtt_port);
  mqtt_client.setCallback(callback);  // assign the input callback
}

void MQTTConnectionManager::connectToMQTT() {
  unsigned long timer = millis();         // prepare the timer
  while (!mqtt_client.connected()) {      // if mqtt is not connected
    if (millis() - timer >= _cooldown) {  // and the timer has hit the required cooldown
      Serial.print("Attempting MQTT connection...");
      timer = millis();
      Serial.printf(_mqtt_username, "clientId-%ld");
      if (mqtt_client.connect(_mqtt_username)) {  // try to connect
        Serial.println(" connected");
      } else {
        Serial.print(" failed, rc=");
        Serial.println(mqtt_client.state());  // if the connection fails print the error code
      }
    }
  }
}

void MQTTConnectionManager::control(std::vector<std::string> themes) {
  if (!mqtt_client.connected()) {  // loop the conection attempts
    connectToMQTT();
    sub(themes);  // subscribe to X themes to receive data
  }
  mqtt_client.loop();  // read the input data
}

void MQTTConnectionManager::control() {
  if (!mqtt_client.connected()) {  // loop the conection attempts
    connectToMQTT();
  }
  mqtt_client.loop();  // read the input data
}

void MQTTConnectionManager::sub(std::vector<std::string> themes) {
  _themes = themes;  // themes to publish
  Serial.println("Subscribed to: ");
  for (auto element : _themes) {
    mqtt_client.subscribe(element.data());
    Serial.println(element.data());
  }
  Serial.println("-----------------------------------");
}

void MQTTConnectionManager::pub(std::string topic, float message) {  // if the published message is a number
  char *topic_c = &topic[0];    // copy the pointer to topic and treat is as a char* (c_char array)
  char buffer[10];
  snprintf(buffer, sizeof buffer, "%f", message);  // convert it to a char buffer
  MQTTConnectionManager::pub(topic_c, buffer);     // and send it
}

void MQTTConnectionManager::pub(std::string topic, int message) {  // if the published message is a number
  char *topic_c = &topic[0];
  strcpy(topic_c, topic.c_str());
  char buffer[10];
  snprintf(buffer, sizeof buffer, "%d", message);  // convert it to a char buffer
  MQTTConnectionManager::pub(topic_c, buffer);     // and send it
}

void MQTTConnectionManager::pub(std::string topic, bool message) {  // if the published message is a boolean
  MQTTConnectionManager::pub(topic, int(message));   // just cast it to integer and send it as is
}

void MQTTConnectionManager::pub(std::string topic, char *message) {
  char *topic_c = &topic[0];
  strcpy(topic_c, topic.c_str());
  mqtt_client.publish(topic_c, message);
}
