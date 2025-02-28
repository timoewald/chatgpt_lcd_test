#include "st7735_display.h"
#include "driver/spi_master.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

static const char *TAG = "st7735_display";

// Definition der Pins (Standard-Pinbelegung, anpassbar)
#define PIN_NUM_MOSI   11
#define PIN_NUM_SCLK   10
#define PIN_NUM_CS     9
#define PIN_NUM_DC     13
#define PIN_NUM_RST    14
#define PIN_NUM_BCKL   21

// Zu verwendender SPI-Host (SPI2)
#define LCD_HOST       SPI2_HOST
#define LCD_PIXEL_CLOCK_HZ (40 * 1000 * 1000)  // 40 MHz

static esp_lcd_panel_handle_t panel_handle = NULL;
static esp_lcd_panel_io_handle_t io_handle = NULL;

/**
 * @brief Callback, der aufgerufen wird, wenn ein SPI-Transfer (über DMA) abgeschlossen ist.
 *
 * Hier wird LVGL signalisiert, dass das Flushen des Displays beendet ist.
 */
static bool st7735_trans_done_cb(esp_lcd_panel_io_handle_t panel_io,
                                   esp_lcd_panel_io_event_data_t *edata, void *user_ctx)
{
    lv_disp_drv_t *disp_drv = (lv_disp_drv_t *)user_ctx;
    lv_disp_flush_ready(disp_drv);
    return false;
}

esp_err_t st7735_display_init(void)
{
    esp_err_t ret;

    // SPI-Bus konfigurieren
    spi_bus_config_t buscfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = -1,
        .sclk_io_num = PIN_NUM_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 160 * 128 * 2, // max. Puffergröße in Bytes (160x128, 2 Byte pro Pixel)
    };
    ret = spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Fehler bei der Initialisierung des SPI-Bus");
        return ret;
    }

    // Panel IO konfigurieren
    esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = PIN_NUM_DC,
        .cs_gpio_num = PIN_NUM_CS,
        .pclk_hz = LCD_PIXEL_CLOCK_HZ,
        .spi_mode = 0,
        .trans_queue_depth = 10,
        .on_color_trans_done = st7735_trans_done_cb,
        .user_ctx = NULL,  // wird später im LVGL-Driver gesetzt
        .flags.dc_as_cmd_phase = 0,
    };

    ret = esp_lcd_new_panel_io_spi(LCD_HOST, &io_config, &io_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Fehler beim Erstellen des Panel IO");
        return ret;
    }

    // Panel-Konfiguration für den ST7735
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = PIN_NUM_RST,
        .color_space = ESP_LCD_COLOR_SPACE_RGB,  // RGB-Farbraum
        .bits_per_pixel = 16,                    // 16 Bit pro Pixel (RGB565)
    };

    ret = esp_lcd_new_panel_st7735(io_handle, &panel_config, &panel_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Fehler beim Erstellen des ST7735-Panels");
        return ret;
    }

    // Panel resetten und initialisieren
    ret = esp_lcd_panel_reset(panel_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Fehler beim Zurücksetzen des Panels");
        return ret;
    }

    ret = esp_lcd_panel_init(panel_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Fehler bei der Initialisierung des Panels");
        return ret;
    }

    // Backlight einschalten (sofern verwendet)
    gpio_set_direction(PIN_NUM_BCKL, GPIO_MODE_OUTPUT);
    gpio_set_level(PIN_NUM_BCKL, 1);

    ESP_LOGI(TAG, "ST7735 Display erfolgreich initialisiert");
    return ESP_OK;
}

void st7735_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map)
{
    int x1 = area->x1;
    int y1 = area->y1;
    int x2 = area->x2;
    int y2 = area->y2;
    // Hinweis: esp_lcd_panel_draw_bitmap erwartet x2 und y2 als exklusiven Endwert!
    esp_err_t ret = esp_lcd_panel_draw_bitmap(panel_handle, x1, y1, x2 + 1, y2 + 1, color_map);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Fehler beim Zeichnen des Bitmaps");
    }
    // lv_disp_flush_ready() wird asynchron im DMA-Callback (st7735_trans_done_cb) aufgerufen!
}
