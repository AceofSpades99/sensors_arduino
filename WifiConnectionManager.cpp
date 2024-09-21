#include <WiFi.h>
#include "WifiConnectionManager.h"

WifiConnectionManager::WifiConnectionManager(const char *ssid) {
  _ssid = ssid;
}

WifiConnectionManager::WifiConnectionManager(const char *ssid, const char *password) {
  _ssid = ssid;
  _password = password;
}

void WifiConnectionManager::begin() {
  // set the wifi to station mode
  WiFi.mode(WIFI_STA);
  // if there's a password attached then use it
  if (_password) {
    WiFi.begin(_ssid, _password);
  } else {
    WiFi.begin(_ssid);
  }

  WifiConnectionManager::connect();
}

void WifiConnectionManager::connect() {
  Serial.println("\nConnecting");
  unsigned long timer = millis();
  // if wifi status isn't connected attempt to reconnect
  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - timer >= 100) {
      timer = millis();
      Serial.print(".");
    }
  }

  // when it does so print the IP
  Serial.println("\nConnected to the WiFi network");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}