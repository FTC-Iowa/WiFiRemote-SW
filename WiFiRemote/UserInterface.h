#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include <Arduino.h>
#include <cstdint>

namespace WiFiRemote {
  extern class __UserInterface {
  public:
    enum class Button {
      Next = 0,
      Previous,
      Start,
      Stop
    };
    
    void begin();

    void run();

    bool buttonEvent() const;
    bool getButtonEvent(Button&);
    bool getButton(Button) const;

    String getButtonName(Button) const;

    bool getLED() const;
    void setLED(bool value);
    void setLEDBlink(unsigned int period);

  private:
    static constexpr uint8_t BUTTON_COUNT = 4;
    
    const uint8_t PIN_BUTTONS[BUTTON_COUNT] = {5, 12, 13, 14};
    const uint8_t PIN_INT = 4;
    const uint8_t PIN_LED = 2;
    const uint8_t DEBOUNCE_PERIOD = 20;

    uint8_t getPin(Button) const;

    static void isr_next();
    static void isr_prev();
    static void isr_start();
    static void isr_stop();
    void isr(Button);

    bool pushEvent(Button b);
    bool popEvent(Button&);

    volatile unsigned int buttonTimes[BUTTON_COUNT];
    volatile bool buttonStates[BUTTON_COUNT];

    //Button change FIFO
    volatile Button buttonEvents[BUTTON_COUNT];
    volatile uint8_t buttonEventLength;

    bool ledState;
    unsigned int blinkPeriod;
    unsigned int lastBlinkTime;
  } UserInterface;
}



#endif
