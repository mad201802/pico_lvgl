/**
 * @file st7789_lvgl_driver.cpp
 * LVGL display driver for ST7789
 */

#include "st7789_lvgl_driver.h"
#include "pico/st7789.h"
#include <cstring>
#include <cstdlib>
#include <stdio.h>

// Display buffer for LVGL
static lv_display_t *display = nullptr;

// Use statically allocated buffers to avoid malloc issues
// Increased buffer size for better stability: 30 rows of 240 pixels
// Buffer size: 240*30 = 7200 pixels = 14400 bytes per buffer (28800 bytes total)
// This provides enough buffer to prevent rendering artifacts
#define DISPLAY_HOR_RES 240
#define STRIP_HEIGHT 30
#define LVGL_BUF_SIZE (DISPLAY_HOR_RES * STRIP_HEIGHT)
static uint16_t draw_buf1[LVGL_BUF_SIZE] __attribute__((aligned(4)));  // RGB565, DMA-aligned
static uint16_t draw_buf2[LVGL_BUF_SIZE] __attribute__((aligned(4)));  // Second buffer for double buffering

/**
 * Flush callback for LVGL - sends the rendered buffer to ST7789
 */
static void st7789_lvgl_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
    uint16_t width = area->x2 - area->x1 + 1;
    uint16_t height = area->y2 - area->y1 + 1;
    
    // CRITICAL: Wait for any previous DMA transfer to complete BEFORE setting new cursor
    // This ensures the previous flush is done before we start a new one
    st7789_wait_dma();
    
    // Set drawing window for this specific area
    st7789_set_cursor(area->x1, area->y1);
    
    // Write pixel data (px_map is in RGB565 format, cast to uint16_t for proper handling)
    st7789_write(px_map, width * height * sizeof(uint16_t));
    
    // CRITICAL: Wait for DMA to complete before telling LVGL we're done
    // This prevents buffer corruption when LVGL starts rendering the next frame
    st7789_wait_dma();
    
    // Tell LVGL we're done flushing and the buffer can be reused
    lv_display_flush_ready(disp);
}

/**
 * Initialize LVGL display driver for ST7789
 */
void st7789_lvgl_init(uint16_t hor_res, uint16_t ver_res) {
    printf("Initializing LVGL...\n");
    
    // Initialize LVGL
    lv_init();
    
    printf("LVGL initialized, creating display...\n");
    printf("Strip rendering mode: %d rows, buffer size: %d pixels (%d bytes)\n", 
           STRIP_HEIGHT, LVGL_BUF_SIZE, sizeof(draw_buf1));
    
    // Create display
    display = lv_display_create(hor_res, ver_res);
    if (!display) {
        printf("ERROR: Failed to create LVGL display!\n");
        return;
    }
    
    printf("Display created, setting up buffers...\n");
    
    // Set flush callback
    lv_display_set_flush_cb(display, st7789_lvgl_flush);
    
    // Set color format to RGB565 (native format for ST7789)
    lv_display_set_color_format(display, LV_COLOR_FORMAT_RGB565);
    
    // Set draw buffers - using DOUBLE buffering with PARTIAL mode for strip rendering
    // Double buffering prevents artifacts: while one buffer is being sent via DMA,
    // LVGL can render the next strip to the other buffer
    lv_display_set_buffers(display, draw_buf1, draw_buf2, 
                          sizeof(draw_buf1), 
                          LV_DISPLAY_RENDER_MODE_PARTIAL);
    
    printf("LVGL display driver initialized successfully!\n");
    printf("RAM optimized: Double-buffered strip rendering (~19KB vs ~23KB before)\n");
}

/**
 * Get millisecond tick for LVGL
 */
uint32_t st7789_lvgl_get_ms(void) {
    return to_ms_since_boot(get_absolute_time());
}
