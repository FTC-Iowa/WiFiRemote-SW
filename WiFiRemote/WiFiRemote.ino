#include "StateMachine.h"

#include <EEPROM.h>
#include "Config.h"

#include "UserInterface.h"

#include <ESP8266WiFi.h>
#include "WiFiStation.h"

#include "Server.h"

using namespace WiFiRemote;
using namespace WiFiRemote::StateMachine;

const int StateCount = 6;
const int EdgeCount = 3;
const int StateStart = 0,
  StateConfigPage = 1,
  StateConnectToAP = 2,
  StateConnectToServer = 3,
  StateWaitForButton = 4,
  StateSendButtonEvent = 5;

FSM<StateCount, EdgeCount> fsm( StateBuilder<EdgeCount>{StateStart,
  []() {
    Serial.println("Enter StateInit");

    Config.begin();
    Serial.println(Config.toString());
    Config.write();
    
    UserInterface.begin();
  },
  []() { },
  []() { } }
  .addTransition(StateConfigPage,
    [&Config]() {
      return !Config ||
        (UserInterface.getButton(__UserInterface::Button::Start) && UserInterface.getButton(__UserInterface::Button::Stop));
  } )
  .addTransition(StateConnectToAP,
    []() { return true; } )
  .build() );

void setup() {
  Serial.begin(115200);
  Serial.println("\nEntering setup");

  buildStateMachine();

  Serial.println("\nLeaving setup");
}

void loop() {
  UserInterface.run();
  fsm.run();
}

void buildStateMachine() {
  fsm.addState( StateBuilder<EdgeCount>{StateConfigPage,
    []() {
      Serial.println("Enter StateConfigPage");
      UserInterface.setLED(true);
    },
    []() { },
    []() { UserInterface.setLED(false); } }
  .addTransition(StateConnectToAP,
    []() { /*On submit valid Config*/ return false; })
  .build() );
  
  fsm.addState( StateBuilder<EdgeCount>{StateConnectToAP,
    []() {
      Serial.println("Enter StateConnectToAP");
      UserInterface.setLEDBlink(500);
      WiFiStation.begin(Config.getSSID(), Config.getPSK());
    },
    []() { },
    []() { UserInterface.setLED(false); } }
  .addTransition(StateConnectToServer,
    []() { return WiFiStation.isConnected(); })
  .addTransition(StateConfigPage,
    []() { return WiFiStation.timedOut(); })
  .build() );

  fsm.addState( StateBuilder<EdgeCount>{StateConnectToServer,
    []() {
      Serial.println("Enter StateConnectToServer");
      UserInterface.setLEDBlink(100);
    },
    []() {
      WiFiRemote::Server.end();
      WiFiRemote::Server.begin(Config.getServerName(), Config.getServerPass());
    },
    []() { UserInterface.setLED(false); } }
  .addTransition(StateConnectToAP,
    []() { return !WiFiStation.isConnected(); })
  .addTransition(StateWaitForButton,
    []() { return WiFiRemote::Server.isConnected(); })
  .build() );
  
  fsm.addState( StateBuilder<EdgeCount>{StateWaitForButton,
    []() { Serial.println("Enter StateWaitForButton"); },
    []() { },
    []() { } }
  .addTransition(StateConnectToAP,
    []() { return !WiFiStation.isConnected(); })
  .addTransition(StateConnectToServer,
    []() { return !WiFiRemote::Server.isConnected(); })
  .addTransition(StateSendButtonEvent,
    []() { return UserInterface.buttonEvent(); })
  .build() );
  fsm.addState( StateBuilder<EdgeCount>{StateSendButtonEvent,
    []() { Serial.println("Enter StateSendButtonEvent"); },
    []() {
      __UserInterface::Button button;
      if(UserInterface.getButtonEvent(button)) {
        Serial.print("Button "); Serial.print(UserInterface.getButtonName(button)); Serial.println(" pressed");
        WiFiRemote::Server.sendEvent(button);
      }
    },
    []() { } }
  .addTransition(StateConnectToAP,
    []() { return !WiFiStation.isConnected(); })
  .addTransition(StateConnectToServer,
    []() { return !WiFiRemote::Server.isConnected(); })
  .addTransition(StateWaitForButton,
    []() { /*After successful transmission of button event to server*/ return true; })
  .build() );
}

