#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "st7735_display.h"
#include "lv_init.h"
#include "lvgl.h"

static const char *TAG = "main";

void app_main(void)
{
    esp_err_t ret;

    // Initialisiere das ST7735-Display
    ret = st7735_display_init();
    if(ret != ESP_OK) {
        ESP_LOGE(TAG, "Display-Initialisierung fehlgeschlagen");
        return;
    }

    // LVGL initialisieren und Display-Treiber registrieren
    lvgl_init();

    // Beispiel: Erstelle ein Label mit Text
    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "Hello, ST7735!");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    // Hauptschleife (die LVGL-Tasks laufen im Hintergrund)
    while(1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
