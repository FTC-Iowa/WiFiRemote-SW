#include "Server.h"

namespace WiFiRemote {
  __Server Server;
}

void WiFiRemote::__Server::begin(const String& name, const String& pass) {
  client.setTimeout(TIMEOUT);
  
  if(client.connect(name.c_str(), PORT)) {
    //TODO: Authenticate with password

    client.print(String("GET /authenticate HTTP/1.1\r\nPassword: ") + pass + "\r\nConnection: keep-alive\r\n\r\n");
  }
}

void WiFiRemote::__Server::end() {
  client.stop();
}

bool WiFiRemote::__Server::isConnected() {
  return client.connected();
}

void WiFiRemote::__Server::sendEvent(__UserInterface::Button b) {
  client.print(String("GET /button-pressed?") + getMAC() + "&btn=" + UserInterface.getButtonName(b) + " HTTP/1.1\r\nConnection: keep-alive\r\n\r\n");
}

String WiFiRemote::__Server::getMAC() {
  auto addr = WiFi.macAddress();
  String str;
  
  for(int i = 0; i < 5; ++i) {
    str += String(addr[i], 16) + ":";
  }
  str += String(addr[5], 16);

  return str;
}

