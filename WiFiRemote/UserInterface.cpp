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

  os_timer_disarm(&blinkTimer);
  os_timer_setfn(&blinkTimer, blinkTimerHandler, this);
  
  pinMode(PIN_LED, OUTPUT);
  setLED(LEDState::Off);
}

void WiFiRemote::__UserInterface::run() {
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

WiFiRemote::__UserInterface::LEDState WiFiRemote::__UserInterface::getLED() const {
  return ledState;
}

void WiFiRemote::__UserInterface::setLED(LEDState value, unsigned int period) {
  if(value == LEDState::Blink) {
    os_timer_arm(&blinkTimer, period, 1);
  }
  else {
    if(ledState == LEDState::Blink) {
      os_timer_disarm(&blinkTimer);
    }
    setLEDPin(!static_cast<bool>(value));
  }
  
  ledState = value;
}

uint8_t WiFiRemote::__UserInterface::getPin(Button b) const {
  return PIN_BUTTONS[static_cast<int>(b)];
}

void WiFiRemote::__UserInterface::setLEDPin(bool value) {
  ledPinValue = value;
  digitalWrite(PIN_LED, value);
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

void WiFiRemote::__UserInterface::blinkTimerHandler(void* self) {
  reinterpret_cast<__UserInterface*>(self)->blinkUpdate();
}

void WiFiRemote::__UserInterface::blinkUpdate() {
  setLEDPin(!ledPinValue);
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

