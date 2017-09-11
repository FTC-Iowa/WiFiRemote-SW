#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include <Arduino.h>
#include <cstdint>

extern "C" {
  #include <os_type.h>
  #include <osapi.h>
}

namespace WiFiRemote {
  extern class __UserInterface {
  public:
    enum class Button {
      Next = 0,
      Previous,
      Start,
      Stop
    };

    enum class LEDState {
      Off = 0,
      On,
      Blink
    };
    
    void begin();

    void run();

    bool buttonEvent() const;
    bool getButtonEvent(Button&);
    bool getButton(Button) const;

    String getButtonName(Button) const;

    LEDState getLED() const;
    void setLED(LEDState, unsigned int period = 0);

  private:
    static constexpr uint8_t BUTTON_COUNT = 4;
    
    const uint8_t PIN_BUTTONS[BUTTON_COUNT] = {5, 12, 13, 14};
    const uint8_t PIN_INT = 4;
    const uint8_t PIN_LED = 2;
    const uint8_t DEBOUNCE_PERIOD = 20;

    uint8_t getPin(Button) const;

    void setLEDPin(bool);

    static void isr_next();
    static void isr_prev();
    static void isr_start();
    static void isr_stop();
    void isr(Button);

    static void blinkTimerHandler(void*);
    void blinkUpdate();

    bool pushEvent(Button b);
    bool popEvent(Button&);

    volatile unsigned int buttonTimes[BUTTON_COUNT];
    volatile bool buttonStates[BUTTON_COUNT];

    //Button change FIFO
    volatile Button buttonEvents[BUTTON_COUNT];
    volatile uint8_t buttonEventLength;

    LEDState ledState;
    bool ledPinValue;
    os_timer_t blinkTimer;
  } UserInterface;

  using LEDState = __UserInterface::LEDState;
}



#endif
