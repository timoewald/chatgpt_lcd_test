#ifndef ST7735_DISPLAY_H
#define ST7735_DISPLAY_H

#include "esp_err.h"
#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialisiert das ST7735-Display.
 *
 * Diese Funktion richtet den SPI-Bus, das Panel-IO und das Panel (ST7735) ein.
 *
 * @return ESP_OK bei Erfolg, ansonsten ein Fehlercode.
 */
esp_err_t st7735_display_init(void);

/**
 * @brief Flush Callback für LVGL.
 *
 * Diese Funktion wird von LVGL aufgerufen, um den gezeichneten Bereich
 * an das Display zu übertragen. Der tatsächliche Flush-Abschluss wird
 * asynchron im DMA-Callback signalisiert.
 *
 * @param drv LVGL Display Driver
 * @param area Der zu aktualisierende Bereich
 * @param color_map Der Farb-Puffer, der an das Display übertragen wird
 */
void st7735_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map);

#ifdef __cplusplus
}
#endif

#endif // ST7735_DISPLAY_H
