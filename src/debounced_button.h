#ifndef DEBOUNCED_BUTTON_H
#define DEBOUNCED_BUTTON_H

#include "pico/stdlib.h"
#include "hardware/gpio.h"

class DebouncedButton {
public:
    DebouncedButton(char label, uint gpio_pin, uint32_t debounce_ms = 50);
    
    // Initialize the GPIO pin
    void init();
    
    // Update button state - call this regularly in main loop
    void update();
    
    // Check if button is currently pressed
    bool isPressed() const { return _current_state; }
    
    // Check if button was just pressed (rising edge)
    bool wasPressed();
    
    // Check if button was just released (falling edge)
    bool wasReleased();
    
    // Get button label
    char getLabel() const { return _label; }
    
    // Get GPIO pin number
    uint getPin() const { return _gpio_pin; }

private:
    char _label;
    uint _gpio_pin;
    uint32_t _debounce_ms;
    
    bool _current_state;
    bool _last_state;
    bool _pressed_event;
    bool _released_event;
    
    uint32_t _last_change_time;
};

#endif // DEBOUNCED_BUTTON_H
