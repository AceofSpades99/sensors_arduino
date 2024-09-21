#include <DHT.h>
#include <HCSR04.h>
#include "WifiConnectionManager.h"
#include "MQTTConnectionManager.h"

// configure wifi
const char *SSID = "Wokwi-GUEST";
const char *PASSWORD = "";
WifiConnectionManager wifi(SSID, PASSWORD);

// configure the mqtt broker
const char *mqtt_broker = "broker.hivemq.com";
const unsigned short port = 1883;
const unsigned long cooldown = 1000;
const char *mqtt_username = "";
const char *mqtt_password = "";

MQTTConnectionManager mqtt(
  mqtt_broker,
  port,
  cooldown,
  mqtt_username,
  mqtt_password
);

#define DHT_PIN 12
#define DHTYPE DHT22
#define HC_TRIG_PIN 18
#define HC_ECHO_PIN 19
#define PIR_PIN 14

DHT dht(DHT_PIN, DHTYPE);
HCSR04 hc(HC_TRIG_PIN, HC_ECHO_PIN);

float DHT_Temperature() {
  // Read temperature
  float t = dht.readTemperature();
  // if it returns NAN then show an error
  if (isnan(t)) {
    Serial.println(F("Error reading temperature!"));
    return -100;
  } else {
    return t;
  }
}

float DHT_Humidity() {
  // Read humidity
  float h = dht.readHumidity();
  if (isnan(h)) {
    Serial.println(F("Error reading humidity!"));
    return -100;
  } else {
    return h;
  }
}

void MQTT_Callback(char *mqtt_topic, byte *payload, unsigned int length) {
  // create a callback to read input data
  Serial.print("Message arrived on topic: ");
  Serial.println(mqtt_topic);

  // construct the data using the char[] that comes from the package payload
  String data;
  for (int i = 0; i < length; i++) {
    data += (char)payload[i];
  }

  Serial.println("-----------------------------------");
}

void setup() {
  Serial.begin(115200);
  // connect to wifi
  wifi.begin();
  // connect to MQTT
  mqtt.begin(MQTT_Callback);
  // DHT
  pinMode(DHT_PIN, INPUT);
  dht.begin();
  // PIR
  pinMode(PIR_PIN, INPUT);
  // HC-SR04
  pinMode(HC_TRIG_PIN, OUTPUT);
  pinMode(HC_ECHO_PIN, INPUT);
}

void loop() {
  // ping and/or reconnect to mqtt
  mqtt.control();

  float temperature = DHT_Temperature();
  float humidity = DHT_Humidity();
  float distance = hc.dist();
  bool movement = digitalRead(PIR_PIN);
  Serial.printf("Temperature: %.2f °C \n", temperature);
  Serial.printf("Humidity: %.2f %% \n", humidity);
  Serial.printf("Distance: %.3f cm \n", distance);  // distance gives 0 if it's >= 395
  if(movement) {
    Serial.println("Detectado movimiento");
  }

  // publish data
  // pub(topic, value)
  mqtt.pub("my_sensors/temperature", temperature);
  mqtt.pub("my_sensors/humidity", humidity);
  mqtt.pub("my_sensors/distance", distance);
  mqtt.pub("my_sensors/movement", movement);

  delay(1000);
}
