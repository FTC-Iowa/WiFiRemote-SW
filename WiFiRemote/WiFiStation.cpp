#include "WiFiStation.h"

#include <Arduino.h>
#include <ESP8266WiFi.h>

namespace WiFiRemote {
  __WiFiStation WiFiStation;
}

void WiFiRemote::__WiFiStation::begin(const String& ssid, const String& psk) {
  timeConnected = millis();
  
  WiFi.begin(ssid.c_str(), psk.c_str());
}

bool WiFiRemote::__WiFiStation::isConnected() const {
  bool connected = WiFi.status() == WL_CONNECTED;

  if(connected) {
    timeConnected = millis();
  }

  return connected;
}

bool WiFiRemote::__WiFiStation::timedOut() const {
  return (millis() - timeConnected) > MAX_CONNECT_TIME;
}

