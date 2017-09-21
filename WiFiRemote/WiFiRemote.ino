#include "StateMachine.h"

#include <EEPROM.h>
#include "Config.h"

#include "UserInterface.h"

#include <ESP8266WiFi.h>
#include "WiFiStation.h"

#include "Server.h"

#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "ConfigPage.h"

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

ConfigPage configPage;

FSM<StateCount, EdgeCount> fsm( StateBuilder<EdgeCount>{StateStart,
  []() {
    Serial.println("Enter StateInit");

    //Disable writing AP settings to flash
    WiFi.persistent(false);

    Config.begin();
    Serial.println(Config.toString());
    
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

      configPage.begin();
      
      UserInterface.setLED(LEDState::On);
    },
    []() { configPage.run(); },
    []() { UserInterface.setLED(LEDState::Off); } }
  .addTransition(StateConnectToAP,
    []() { /*On submit valid Config*/ return false; })
  .build() );
  
  fsm.addState( StateBuilder<EdgeCount>{StateConnectToAP,
    []() {
      Serial.println("Enter StateConnectToAP");
      UserInterface.setLED(LEDState::Blink, 500);
      WiFiStation.begin(Config.getSSID(), Config.getPSK());
    },
    []() { },
    []() { UserInterface.setLED(LEDState::Off); } }
  .addTransition(StateConnectToServer,
    []() { return WiFiStation.isConnected(); })
  .addTransition(StateConfigPage,
    []() { return WiFiStation.timedOut(); })
  .build() );

  fsm.addState( StateBuilder<EdgeCount>{StateConnectToServer,
    []() {
      Serial.println("Enter StateConnectToServer");
      UserInterface.setLED(LEDState::Blink, 100);
    },
    []() {
      WiFiRemote::Server.end();
      WiFiRemote::Server.begin(Config.getServerName(), Config.getServerPass());
    },
    []() { UserInterface.setLED(LEDState::Off); } }
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

