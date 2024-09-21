#ifndef WifiConnectionManager_h
#define WifiConnectionManager_h

#include "Arduino.h"

class WifiConnectionManager {
  private:
    void connect();
    const char *_ssid;
    const char *_password;
  public:
    WifiConnectionManager(const char *ssid);
    WifiConnectionManager(const char *ssid, const char *password);
    void begin();
};
#endif