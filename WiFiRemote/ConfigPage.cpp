#include "ConfigPage.h"

#include <ESP8266WiFi.h>
#include "Config.h"

namespace WiFiRemote {
  extern char* htmlConfigIndex;
  extern char* htmlConfigServer;
  extern char* htmlConfigDone;
}

WiFiRemote::ConfigPage::ConfigPage()
  : webServer{80} {
}

void WiFiRemote::ConfigPage::begin() {
  WiFi.mode(WIFI_AP_STA);
  WiFi.disconnect();
  WiFi.softAP(String(String(AP_NAME) + " " + Config.getMAC()).c_str());
  
  webServer.on("/", [this]() {
    webServer.send(200, "text/html", htmlConfigIndex);
  });
  webServer.on("/config_server.html", [this]() {
    if(WiFi.status() == WL_CONNECTED) {
      webServer.send(200, "text/html", htmlConfigServer);
    }
    else {
      //Redirect to start page
      webServer.sendHeader("Location", "/", true);
      webServer.send(302, "text/plain", "");
    }
  });
  webServer.on("/config_done.html", [this]() {
    webServer.send(200, "text/html", htmlConfigDone);
  });

  //REST endpoints
  webServer.on("/station_scan", [this]() {
    int count = WiFi.scanNetworks();
    String result = "{\"Stations\":[";

    for(int i = 0; i < count; ++i) {
      result += '\"' + WiFi.SSID(i) + '\"';
      if(i < (count-1)) {
        result += ',';
      }
    }
    result += "]}";

    webServer.send(200, "text/plain", result);
  });
  webServer.on("try_connect", [this]() {
    bool success = false;
    
    if(webServer.hasHeader("ssid")) {
      String ssid = webServer.header("ssid");
      if(webServer.hasHeader("psk")) {
        String psk = webServer.header("psk");

        WiFi.begin(ssid.c_str(), psk.c_str());
        auto status = WiFi.waitForConnectResult();

        if(status == WL_CONNECTED) {
          success = true;
        }
      }
      webServer.send(200, "text/plain", String("{\"success\":\"") + (success ? "true" : "false") + "\"}");
    }
  });

  webServer.begin();
  MDNS.begin(HOST_NAME);
}

void WiFiRemote::ConfigPage::run() {
  webServer.handleClient();
}


