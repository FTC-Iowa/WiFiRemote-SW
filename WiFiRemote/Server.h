#ifndef SERVER_H
#define SERVER_H

#include <Arduino.h>
#include <ESP8266WiFi.h>

#include "UserInterface.h"

namespace WiFiRemote {
  extern class __Server {
  public:

    void begin(const String& name, const String& pass);
    void end();

    bool isConnected();

    void sendEvent(__UserInterface::Button);

  private:
    static const uint16_t PORT = 8080;
    static const int TIMEOUT = 100;

    WiFiClient client;

    String getMAC();
    
  } Server;
}

#endif
