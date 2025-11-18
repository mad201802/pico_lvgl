#include "pico/stdlib.h"
#include "pico/st7789.h"
#include "hardware/spi.h"
#include "lvgl.h"
#include "st7789_lvgl_driver.h"
#include "debounced_button.h"
#include <stdio.h>

// ST7789 display configuration
// Adjust these pins according to your hardware setup
#define ST7789_SPI_PORT spi0
#define ST7789_PIN_DIN  19
#define ST7789_PIN_CLK  18
#define ST7789_PIN_CS   17
#define ST7789_PIN_DC   16
#define ST7789_PIN_RST  20
#define ST7789_PIN_BL   21

// Display dimensions (adjust for your display)
#define DISPLAY_WIDTH  240
#define DISPLAY_HEIGHT 240

// Hardware buttons configuration
DebouncedButton _buttons[] = {
    DebouncedButton('A', 12),
    DebouncedButton('B', 13),
    DebouncedButton('X', 14),
    DebouncedButton('Y', 15),
};

const int NUM_BUTTONS = sizeof(_buttons) / sizeof(_buttons[0]);

// UI elements
lv_obj_t *button_labels[4];
lv_obj_t *counter_label;

// LVGL tick timer callback
bool lvgl_timer_callback(struct repeating_timer *t) {
    lv_tick_inc(5);  // Increment LVGL tick by 5ms
    return true;
}

void create_simple_ui() {
    printf("Creating UI...\n");
    
    // Set background color to dark blue
    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x003a57), LV_PART_MAIN);
    
    // Create a title label
    lv_obj_t *title = lv_label_create(lv_screen_active());
    lv_label_set_text(title, "Button Demo");
    lv_obj_set_style_text_color(title, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);
    
    // Create a container for button status
    lv_obj_t *container = lv_obj_create(lv_screen_active());
    lv_obj_set_size(container, 200, 140);
    lv_obj_align(container, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_bg_color(container, lv_color_hex(0x1a1a1a), LV_PART_MAIN);
    lv_obj_set_style_border_color(container, lv_color_hex(0x00aaff), LV_PART_MAIN);
    lv_obj_set_style_border_width(container, 2, LV_PART_MAIN);
    
    // Create labels for each button
    const char* button_names[] = {"A", "B", "X", "Y"};
    for (int i = 0; i < 4; i++) {
        button_labels[i] = lv_label_create(container);
        char buf[32];
        snprintf(buf, sizeof(buf), "%s: Released", button_names[i]);
        lv_label_set_text(button_labels[i], buf);
        lv_obj_set_style_text_color(button_labels[i], lv_color_hex(0xCCCCCC), LV_PART_MAIN);
        lv_obj_align(button_labels[i], LV_ALIGN_TOP_LEFT, 10, 10 + i * 30);
    }
    
    // Create a counter label at the bottom
    counter_label = lv_label_create(lv_screen_active());
    lv_label_set_text(counter_label, "Press Count: 0");
    lv_obj_set_style_text_color(counter_label, lv_color_hex(0x00FF00), LV_PART_MAIN);
    lv_obj_align(counter_label, LV_ALIGN_BOTTOM_MID, 0, -10);
    
    printf("UI creation complete!\n");
}

int main() {
    // Initialize stdio FIRST
    stdio_init_all();
    
    // Wait for USB serial
    // sleep_ms(2000);
    
    printf("Initializing hardware buttons...\n");
    
    // Initialize hardware buttons
    for (int i = 0; i < NUM_BUTTONS; i++) {
        _buttons[i].init();
        printf("Button %c initialized on GPIO %d\n", _buttons[i].getLabel(), _buttons[i].getPin());
    }
    
    printf("Initializing ST7789 display...\n");
    
    // Configure ST7789 display with DMA enabled
    struct st7789_config config = {
        .spi = ST7789_SPI_PORT,
        .gpio_din = ST7789_PIN_DIN,
        .gpio_clk = ST7789_PIN_CLK,
        .gpio_cs = ST7789_PIN_CS,
        .gpio_dc = ST7789_PIN_DC,
        .gpio_rst = ST7789_PIN_RST,
        .gpio_bl = ST7789_PIN_BL,
        .use_dma = true  // Enable DMA for better performance
    };
    
    // Initialize the ST7789 display
    st7789_init(&config, DISPLAY_WIDTH, DISPLAY_HEIGHT);
    printf("ST7789 initialized OK with DMA enabled\n");
    
    // Clear display to black first
    printf("Clearing display...\n");
    st7789_fill(0x0000);
    sleep_ms(500);
    
    // Initialize LVGL with ST7789 driver
    printf("Initializing LVGL driver...\n");
    st7789_lvgl_init(DISPLAY_WIDTH, DISPLAY_HEIGHT);
    printf("LVGL driver initialized OK\n");
    
    // Set up timer for LVGL tick
    printf("Setting up LVGL timer...\n");
    struct repeating_timer timer;
    add_repeating_timer_ms(5, lvgl_timer_callback, NULL, &timer);
    printf("Timer initialized OK\n");
    
    // Create demo UI
    printf("Creating demo UI...\n");
    create_simple_ui();
    
    // Force first render
    printf("Forcing initial render...\n");
    lv_timer_handler();
    printf("Initial render complete\n");
    
    printf("Entering main loop...\n");
    
    uint32_t press_count = 0;
    const char* button_names[] = {"A", "B", "X", "Y"};
    
    while (true) {
        // Update all buttons
        for (int i = 0; i < NUM_BUTTONS; i++) {
            _buttons[i].update();
            
            // Check for button press events
            if (_buttons[i].wasPressed()) {
                press_count++;
                printf("Button %c pressed! (Count: %d)\n", _buttons[i].getLabel(), press_count);
                
                // Update UI - change label to green and show "Pressed"
                char buf[32];
                snprintf(buf, sizeof(buf), "%s: Pressed", button_names[i]);
                lv_label_set_text(button_labels[i], buf);
                lv_obj_set_style_text_color(button_labels[i], lv_color_hex(0x00FF00), LV_PART_MAIN);
                
                // Update counter
                snprintf(buf, sizeof(buf), "Press Count: %d", press_count);
                lv_label_set_text(counter_label, buf);
                
                // Invalidate the affected objects to force clean redraw
                lv_obj_invalidate(button_labels[i]);
                lv_obj_invalidate(counter_label);
            }
            
            // Check for button release events
            if (_buttons[i].wasReleased()) {
                printf("Button %c released!\n", _buttons[i].getLabel());
                
                // Update UI - change label back to gray and show "Released"
                char buf[32];
                snprintf(buf, sizeof(buf), "%s: Released", button_names[i]);
                lv_label_set_text(button_labels[i], buf);
                lv_obj_set_style_text_color(button_labels[i], lv_color_hex(0xCCCCCC), LV_PART_MAIN);
                
                // Invalidate to force clean redraw
                lv_obj_invalidate(button_labels[i]);
            }
        }
        
        // Handle LVGL tasks - this processes all rendering
        lv_timer_handler();
        
        // Small delay - LVGL timer handler runs quickly with DMA
        sleep_ms(5);
    }
}