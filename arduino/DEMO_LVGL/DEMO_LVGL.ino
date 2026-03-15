#include <Arduino.h>
#include <lvgl.h>
#include "display.h"
#include "esp_bsp.h"
#include "lv_port.h"
#include "ui.h"
#include "wifi_manager.h"
#include "sd_manager.h"

void setup() {
    Serial.begin(115200);

    bsp_display_cfg_t cfg = {
        .lvgl_port_cfg = ESP_LVGL_PORT_INIT_CONFIG(),
        .buffer_size = EXAMPLE_LCD_QSPI_H_RES * EXAMPLE_LCD_QSPI_V_RES,
        .rotate = LV_DISP_ROT_90,
    };
    bsp_display_start_with_config(&cfg);
    bsp_display_backlight_on();

    // WiFi와 SD 초기화는 디스플레이 잠금 없이 수행 (내부에서 리소스 로딩 시간이 걸릴 수 있음)
    wifi_mgr_init();
    sd_mgr_init();

    // UI 객체 생성은 디스플레이 잠금을 유지해야 함
    bsp_display_lock(0);
    ui_init();
    bsp_display_unlock();
}

void loop() {
    delay(1000);
}