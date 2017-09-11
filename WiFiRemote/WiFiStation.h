#ifndef WIFI_STATION_H
#define WIFI_STATION_H

#include <Arduino.h>

namespace WiFiRemote {
  extern class __WiFiStation {
  public:
    void begin(const String& ssid, const String& psk);
    
    bool isConnected() const;
    bool timedOut() const;

  private:
    static const unsigned int MAX_CONNECT_TIME = 10000; //milliseconds
    mutable unsigned int timeConnected;
    
  } WiFiStation;
}

#endif
