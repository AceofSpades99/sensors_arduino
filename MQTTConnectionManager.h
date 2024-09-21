#ifndef MQTTConnectionManager_h
#define MQTTConnectionManager_h

#include <iostream>
#include <vector>
#include "Arduino.h"

class MQTTConnectionManager {
  private:
    const char *_mqtt_broker;
    unsigned short _mqtt_port;
    unsigned long _cooldown;
    const char *_mqtt_username;
    const char *_mqtt_password;
    std::vector<std::string> _themes;
  public:
    MQTTConnectionManager(const char *mqtt_broker,
                          const unsigned short mqtt_port,
                          const unsigned long cooldown,
                          const char *mqtt_username,
                          const char *mqtt_password
                         );
    void begin(void (*callback)(char*, byte*, unsigned int));
    void connectToMQTT();
    void control(std::vector<std::string> themes);
    void control();
    void pub(std::string topic, char *message);
    void pub(std::string topic, float message);
    void pub(std::string topic, int message);
    void pub(std::string topic, bool message);
    void sub(std::vector<std::string> themes);
};
#endif