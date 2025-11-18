/**
 * @file st7789_lvgl_driver.h
 * LVGL display driver header for ST7789
 */

#ifndef ST7789_LVGL_DRIVER_H
#define ST7789_LVGL_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"
#include "pico/stdlib.h"

/**
 * Initialize LVGL display driver for ST7789
 * @param hor_res Horizontal resolution
 * @param ver_res Vertical resolution
 */
void st7789_lvgl_init(uint16_t hor_res, uint16_t ver_res);

/**
 * Get millisecond tick for LVGL
 * @return Current time in milliseconds
 */
uint32_t st7789_lvgl_get_ms(void);

#ifdef __cplusplus
}
#endif

#endif // ST7789_LVGL_DRIVER_H
