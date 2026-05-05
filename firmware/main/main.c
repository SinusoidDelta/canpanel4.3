#include <stdbool.h>
#include <stdint.h>

#include "driver/i2c_master.h"
#include "esp_check.h"
#include "esp_err.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_rgb.h"
#include "esp_lcd_touch_gt911.h"
#include "esp_log.h"
#include "esp_lvgl_port.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lvgl.h"

#include "fallback_ui.h"

#if GENERATED_UI_MODE == 1
#include "ui.h"
#elif GENERATED_UI_MODE == 2
#include "main_screen_gen.h"
#endif

#define LCD_H_RES 800
#define LCD_V_RES 480
#define LCD_PIXEL_CLOCK_HZ (13 * 1000 * 1000)
#define LCD_BUF_LINES 48

#define PIN_LCD_PCLK 7
#define PIN_LCD_VSYNC 3
#define PIN_LCD_HSYNC 46
#define PIN_LCD_DE 5
#define PIN_LCD_DISP -1
#define PIN_LCD_DATA0 1
#define PIN_LCD_DATA1 2
#define PIN_LCD_DATA2 42
#define PIN_LCD_DATA3 41
#define PIN_LCD_DATA4 40
#define PIN_LCD_DATA5 39
#define PIN_LCD_DATA6 0
#define PIN_LCD_DATA7 45
#define PIN_LCD_DATA8 48
#define PIN_LCD_DATA9 47
#define PIN_LCD_DATA10 21
#define PIN_LCD_DATA11 14
#define PIN_LCD_DATA12 38
#define PIN_LCD_DATA13 18
#define PIN_LCD_DATA14 17
#define PIN_LCD_DATA15 10

#define PIN_TOUCH_SDA 8
#define PIN_TOUCH_SCL 9
#define PIN_TOUCH_INT 4
#define PIN_TOUCH_RST -1

static const char *TAG = "canpanel43";

static esp_err_t init_rgb_panel(esp_lcd_panel_handle_t *panel)
{
    esp_lcd_rgb_panel_config_t panel_config = {
        .clk_src = LCD_CLK_SRC_DEFAULT,
        .timings = {
            .pclk_hz = LCD_PIXEL_CLOCK_HZ,
            .h_res = LCD_H_RES,
            .v_res = LCD_V_RES,
            .hsync_pulse_width = 10,
            .hsync_back_porch = 10,
            .hsync_front_porch = 10,
            .vsync_pulse_width = 10,
            .vsync_back_porch = 10,
            .vsync_front_porch = 20,
            .flags.pclk_active_neg = false,
        },
        .data_width = 16,
        .bits_per_pixel = 16,
        .num_fbs = 2,
        .bounce_buffer_size_px = LCD_H_RES * LCD_BUF_LINES,
        .sram_trans_align = 4,
        .psram_trans_align = 64,
        .hsync_gpio_num = PIN_LCD_HSYNC,
        .vsync_gpio_num = PIN_LCD_VSYNC,
        .de_gpio_num = PIN_LCD_DE,
        .pclk_gpio_num = PIN_LCD_PCLK,
        .disp_gpio_num = PIN_LCD_DISP,
        .data_gpio_nums = {
            PIN_LCD_DATA0, PIN_LCD_DATA1, PIN_LCD_DATA2, PIN_LCD_DATA3,
            PIN_LCD_DATA4, PIN_LCD_DATA5, PIN_LCD_DATA6, PIN_LCD_DATA7,
            PIN_LCD_DATA8, PIN_LCD_DATA9, PIN_LCD_DATA10, PIN_LCD_DATA11,
            PIN_LCD_DATA12, PIN_LCD_DATA13, PIN_LCD_DATA14, PIN_LCD_DATA15,
        },
        .flags.fb_in_psram = true,
    };

    ESP_RETURN_ON_ERROR(esp_lcd_new_rgb_panel(&panel_config, panel), TAG, "RGB panel allocation failed");
    ESP_RETURN_ON_ERROR(esp_lcd_panel_reset(*panel), TAG, "RGB panel reset failed");
    return esp_lcd_panel_init(*panel);
}

static esp_err_t init_touch(esp_lcd_touch_handle_t *touch)
{
    i2c_master_bus_handle_t i2c_bus = NULL;
    i2c_master_bus_config_t bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_NUM_0,
        .sda_io_num = PIN_TOUCH_SDA,
        .scl_io_num = PIN_TOUCH_SCL,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    ESP_RETURN_ON_ERROR(i2c_new_master_bus(&bus_config, &i2c_bus), TAG, "I2C bus init failed");

    esp_lcd_panel_io_handle_t tp_io = NULL;
    esp_lcd_panel_io_i2c_config_t tp_io_config = ESP_LCD_TOUCH_IO_I2C_GT911_CONFIG();
    ESP_RETURN_ON_ERROR(esp_lcd_new_panel_io_i2c(i2c_bus, &tp_io_config, &tp_io), TAG, "touch IO init failed");

    esp_lcd_touch_config_t tp_cfg = {
        .x_max = LCD_H_RES,
        .y_max = LCD_V_RES,
        .rst_gpio_num = PIN_TOUCH_RST,
        .int_gpio_num = PIN_TOUCH_INT,
        .levels = {
            .reset = 0,
            .interrupt = 0,
        },
        .flags = {
            .swap_xy = false,
            .mirror_x = false,
            .mirror_y = false,
        },
    };

    return esp_lcd_touch_new_i2c_gt911(tp_io, &tp_cfg, touch);
}

static esp_err_t init_lvgl(esp_lcd_panel_handle_t panel, esp_lcd_touch_handle_t touch)
{
    const lvgl_port_cfg_t lvgl_cfg = ESP_LVGL_PORT_INIT_CONFIG();
    ESP_RETURN_ON_ERROR(lvgl_port_init(&lvgl_cfg), TAG, "LVGL port init failed");

    const lvgl_port_display_cfg_t disp_cfg = {
        .io_handle = NULL,
        .panel_handle = panel,
        .buffer_size = LCD_H_RES * LCD_BUF_LINES,
        .double_buffer = true,
        .hres = LCD_H_RES,
        .vres = LCD_V_RES,
        .monochrome = false,
        .color_format = LV_COLOR_FORMAT_RGB565,
        .rotation = {
            .swap_xy = false,
            .mirror_x = false,
            .mirror_y = false,
        },
        .flags = {
            .buff_dma = false,
            .buff_spiram = true,
            .sw_rotate = false,
            .swap_bytes = true,
        },
    };
    lv_disp_t *disp = lvgl_port_add_disp(&disp_cfg);
    ESP_RETURN_ON_FALSE(disp != NULL, ESP_FAIL, TAG, "LVGL display add failed");

    const lvgl_port_touch_cfg_t touch_cfg = {
        .disp = disp,
        .handle = touch,
    };
    ESP_RETURN_ON_FALSE(lvgl_port_add_touch(&touch_cfg) != NULL, ESP_FAIL, TAG, "LVGL touch add failed");

    return ESP_OK;
}

void app_main(void)
{
    esp_lcd_panel_handle_t panel = NULL;
    esp_lcd_touch_handle_t touch = NULL;

    ESP_ERROR_CHECK(init_rgb_panel(&panel));
    ESP_ERROR_CHECK(init_touch(&touch));
    ESP_ERROR_CHECK(init_lvgl(panel, touch));

    if (lvgl_port_lock(0)) {
#if GENERATED_UI_MODE == 1
        ESP_LOGI(TAG, "Loading generated LVGL UI from ui.c");
        ui_init();
#elif GENERATED_UI_MODE == 2
        ESP_LOGI(TAG, "Loading generated LVGL screen from main_screen_gen.c");
        lv_obj_t *screen = main_screen_create();
        lv_screen_load(screen);
#else
        ESP_LOGI(TAG, "Loading fallback LVGL UI; run firmware/tools/sync_ui.sh after XML generation");
        fallback_ui_load();
#endif
        lvgl_port_unlock();
    }

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
