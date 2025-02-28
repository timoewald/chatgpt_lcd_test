#include "lv_init.h"
#include "lvgl.h"
#include "st7735_display.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char *TAG = "lv_init";

#define LV_HOR_RES 160
#define LV_VER_RES 128

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf1[LV_HOR_RES * 20];  // Puffer für 20 Zeilen

/**
 * @brief Task, der den LVGL-Tick erhöht.
 */
static void lv_tick_task(void *arg)
{
    while (1) {
        lv_tick_inc(10); // Erhöhe den LVGL Tick um 10 ms
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

/**
 * @brief Task, der den LVGL-Timer-Handler aufruft.
 */
static void lv_task_handler(void *arg)
{
    while (1) {
        lv_timer_handler();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void lvgl_init(void)
{
    ESP_LOGI(TAG, "LVGL wird initialisiert");
    lv_init();

    // Zeichnungspuffer initialisieren
    lv_disp_draw_buf_init(&draw_buf, buf1, NULL, LV_HOR_RES * 20);

    // LVGL Display-Treiber initialisieren
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = LV_HOR_RES;
    disp_drv.ver_res = LV_VER_RES;
    disp_drv.draw_buf = &draw_buf;
    disp_drv.flush_cb = st7735_flush_cb;
    // Übergabe des Display-Treiber-Zeigers über user_data (wird im DMA-Callback genutzt)
    disp_drv.user_data = &disp_drv;
    lv_disp_drv_register(&disp_drv);

    // Tasks für LVGL-Tick und Handler erstellen
    xTaskCreate(lv_tick_task, "lv_tick", 1024, NULL, 5, NULL);
    xTaskCreate(lv_task_handler, "lv_handler", 4096, NULL, 5, NULL);
}
