#ifndef CONFIG_PAGE_H
#define CONFIG_PAGE_H

#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

namespace WiFiRemote {
  class ConfigPage {
  public:
    ConfigPage();
  
    void begin();
    void end();
  
    void run();
  private:
    const char* AP_NAME = "WiFi Remote";
    const char* HOST_NAME = "wifi-remote";
    ESP8266WebServer webServer;
  };
}

#endif
