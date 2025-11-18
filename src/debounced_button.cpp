#include "debounced_button.h"

DebouncedButton::DebouncedButton(char label, uint gpio_pin, uint32_t debounce_ms)
    : _label(label)
    , _gpio_pin(gpio_pin)
    , _debounce_ms(debounce_ms)
    , _current_state(false)
    , _last_state(false)
    , _pressed_event(false)
    , _released_event(false)
    , _last_change_time(0)
{
}

void DebouncedButton::init() {
    // Configure GPIO as input with pull-up resistor
    // Buttons should connect GPIO to GND when pressed
    gpio_init(_gpio_pin);
    gpio_set_dir(_gpio_pin, GPIO_IN);
    gpio_pull_up(_gpio_pin);
}

void DebouncedButton::update() {
    // Read current pin state (active low - pressed = 0)
    bool raw_state = !gpio_get(_gpio_pin);
    
    // Get current time
    uint32_t current_time = to_ms_since_boot(get_absolute_time());
    
    // Clear edge events
    _pressed_event = false;
    _released_event = false;
    
    // Check if state has changed
    if (raw_state != _last_state) {
        _last_change_time = current_time;
        _last_state = raw_state;
    }
    
    // Update stable state after debounce period
    if ((current_time - _last_change_time) >= _debounce_ms) {
        if (raw_state != _current_state) {
            // State has changed and is stable
            if (raw_state) {
                _pressed_event = true;
            } else {
                _released_event = true;
            }
            _current_state = raw_state;
        }
    }
}

bool DebouncedButton::wasPressed() {
    bool result = _pressed_event;
    _pressed_event = false;  // Clear event after reading
    return result;
}

bool DebouncedButton::wasReleased() {
    bool result = _released_event;
    _released_event = false;  // Clear event after reading
    return result;
}
