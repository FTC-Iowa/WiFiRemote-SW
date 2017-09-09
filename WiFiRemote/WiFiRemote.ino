#include "StateMachine.h"

#include <EEPROM.h>
#include "Config.h"

using namespace WiFiRemote::StateMachine;

const int StateCount = 5;
const int EdgeCount = 2;
const int StateStart = 0,
  StateConfigPage = 1,
  StateConnectToAP = 2,
  StateWaitForButton = 3,
  StateSendButtonEvent = 4;

WiFiRemote::Config config;

FSM<StateCount, EdgeCount> fsm( StateBuilder<EdgeCount>{StateStart,
  []() { Serial.println("Enter State 0"); },
  []() { /*Do Nothing*/ },
  []() { /*Do Nothing*/ } }
  .addTransition(StateConfigPage,
    [&config]() {
      /*If buttons held*/
      return !config;
  } )
  .addTransition(StateConnectToAP,
    [&config]() { /*If buttons not held*/ return !!config; } )
  .build() );

void setup() {
  Serial.begin(115200);
  Serial.println("\nEntering setup");

  fsm.addState( StateBuilder<EdgeCount>{StateConfigPage,
    []() { Serial.println("Enter State 1"); },
    []() { },
    []() { } }
  .addTransition(StateConnectToAP,
    []() { /*On submit valid config*/ return false; })
  .build() );
  
  fsm.addState( StateBuilder<EdgeCount>{StateConnectToAP,
    []() { Serial.println("Enter State 2"); },
    []() { },
    []() { } }
  .addTransition(StateConfigPage,
    []() { /*Failed to connect / authenticate with server*/ return false; })
  .addTransition(StateWaitForButton,
    []() { /*On connected/authenticated with server*/ return false; })
  .build() );
  
  fsm.addState( StateBuilder<EdgeCount>{StateWaitForButton,
    []() { Serial.println("Enter State 3"); },
    []() { },
    []() { } }
  .addTransition(StateConnectToAP,
    []() { /*On disconnect from AP*/ return false; })
  .addTransition(StateSendButtonEvent,
    []() { /*On button press event*/ return false; })
  .build() );
  
  fsm.addState( StateBuilder<EdgeCount>{StateSendButtonEvent,
    []() { Serial.println("Enter State 4"); },
    []() { },
    []() { } }
  .addTransition(StateConnectToAP,
    []() { /*On disconnect from AP*/ return false; })
  .addTransition(StateWaitForButton,
    []() { /*After successful transmission of button event to server*/ return false; })
  .build() );

  Serial.println("\nLeaving setup");
}

void loop() {
  fsm.run();
}
