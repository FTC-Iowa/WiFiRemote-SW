#include "UserInterface.h"

namespace WiFiRemote {
  __UserInterface UserInterface;
}

void WiFiRemote::__UserInterface::begin() {
  buttonEventLength = 0;
  
  for(int i = 0; i < BUTTON_COUNT; ++i) {
    auto pin = PIN_BUTTONS[i];
    
    pinMode(pin, INPUT_PULLUP);
    buttonStates[i] = digitalRead(pin);
    buttonTimes[i] = 0;

    Serial.print("Setting GPIO "); Serial.print(pin); Serial.println(" to input");
  }

  attachInterrupt(getPin(Button::Next), isr_next, CHANGE);
  attachInterrupt(getPin(Button::Previous), isr_prev, CHANGE);
  attachInterrupt(getPin(Button::Start), isr_start, CHANGE);
  attachInterrupt(getPin(Button::Stop), isr_stop, CHANGE);
  
  pinMode(PIN_LED, OUTPUT);
  setLED(ledState);
}

void WiFiRemote::__UserInterface::run() {
  if(blinkPeriod != 0) {
    unsigned int curTime = millis();

    Serial.println("RUN");
  
    if( (curTime - lastBlinkTime) >= blinkPeriod ) {
      Serial.println("BLINK");
      setLED(!getLED());
      lastBlinkTime = curTime;
    }
  }
}

bool WiFiRemote::__UserInterface::buttonEvent() const {
  return buttonEventLength > 0;
}

bool WiFiRemote::__UserInterface::getButtonEvent(Button& b) {
  return popEvent(b);
}

bool WiFiRemote::__UserInterface::getButton(Button b) const {
  //return !buttonStates[static_cast<int>(b)]; //Button is active low
  return !digitalRead(PIN_BUTTONS[static_cast<int>(b)]);
}

String WiFiRemote::__UserInterface::getButtonName(Button b) const {
  const char* BUTTON_NAMES[] = {"Next", "Previous", "Start", "Stop"};

  static_assert((sizeof(BUTTON_NAMES)/sizeof(BUTTON_NAMES[0])) == (sizeof(PIN_BUTTONS)/sizeof(PIN_BUTTONS[0])),
    "Different number of buttons in PIN_BUTTONS array and BUTTON_NAMES array");

  return BUTTON_NAMES[static_cast<int>(b)];
}

bool WiFiRemote::__UserInterface::getLED() const {
  return ledState;
}

void WiFiRemote::__UserInterface::setLED(bool value) {
  ledState = value;
  blinkPeriod = 0;
  digitalWrite(PIN_LED, !value); //LED is active low
}

void WiFiRemote::__UserInterface::setLEDBlink(unsigned int period) {
  blinkPeriod = period;
  lastBlinkTime = millis();
}

uint8_t WiFiRemote::__UserInterface::getPin(Button b) const {
  return PIN_BUTTONS[static_cast<int>(b)];
}

void WiFiRemote::__UserInterface::isr_next() {
  UserInterface.isr(Button::Next);
}
void WiFiRemote::__UserInterface::isr_prev() {
  UserInterface.isr(Button::Previous);
}
void WiFiRemote::__UserInterface::isr_start() {
  UserInterface.isr(Button::Start);
}
void WiFiRemote::__UserInterface::isr_stop() {
  UserInterface.isr(Button::Stop);
}

void WiFiRemote::__UserInterface::isr(Button b) {
  auto bID = static_cast<int>(b);

  auto curTime = millis();
  bool newState = digitalRead(getPin(b));
  
  if( (buttonTimes[bID] < (curTime - DEBOUNCE_PERIOD)) && (newState != buttonStates[bID]) ) {
    buttonStates[bID] = newState;
    buttonTimes[bID] = curTime;

    if(!newState) {
      pushEvent(b);
    }
  }
}

bool WiFiRemote::__UserInterface::pushEvent(Button b) {
  bool buttonFound = false;
  for(int i = 0; i < buttonEventLength; ++i) {
    if(buttonEvents[i] == b) {
      buttonFound = true;
      break;
    }
  }

  if(!buttonFound) {
    buttonEvents[buttonEventLength++] = b;

    return true;
  }

  return false;
}

bool WiFiRemote::__UserInterface::popEvent(Button& b) {
  if(buttonEventLength > 0) {
    b = buttonEvents[0];

    memmove(const_cast<void*>(static_cast<volatile void*>(buttonEvents)),
      const_cast<void*>(static_cast<volatile void*>(&(buttonEvents[1]))),
      sizeof(Button) * (buttonEventLength-1));
    buttonEventLength--;

    return true;
  }

  return false;
}

