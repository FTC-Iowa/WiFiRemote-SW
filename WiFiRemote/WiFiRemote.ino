#include "StateMachine.h"

using namespace StateMachine;

const int StateCount = 5;
const int EdgeCount = 2;

const int StateStart = 0,
  StateConfigPage = 1,
  StateConnectToAP = 2,
  StateWaitForButton = 3,
  StateSendButtonEvent = 4;

FSM<StateCount, EdgeCount> fsm( StateBuilder<EdgeCount>{StateStart,
  []() { /*Read buttons*/ },
  []() { /*Do Nothing*/ },
  []() { /*Do Nothing*/ } }
  .addTransition(StateConfigPage,
    []() { /*If buttons held*/ return false; } )
  .addTransition(StateConnectToAP,
    []() { /*If buttons not held*/ return false; } )
  .build() );

void setup() {
  
}

void loop() {
  fsm.run();
}
