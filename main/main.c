#include <string.h>

#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "config.h"
#include "csi.h"
#include "wifi.h"

void app_main(void)
{
    esp_log_level_set("*", ESP_LOG_NONE);

    uart_config_t uart_cfg = {
        .baud_rate  = CSI_BAUD,
        .data_bits  = UART_DATA_8_BITS,
        .parity     = UART_PARITY_DISABLE,
        .stop_bits  = UART_STOP_BITS_1,
        .flow_ctrl  = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    ESP_ERROR_CHECK(uart_driver_install(CSI_UART, 4096, 4096, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(CSI_UART, &uart_cfg));
    ESP_ERROR_CHECK(uart_set_pin(CSI_UART, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE,
                                  UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    const char *banner =
        "\nREADY,CSI_RADAR,921600\n"
        "FORMAT:CSI,ts_us,rssi,ch,len,i0,q0,i1,q1,...\n";
    uart_write_bytes(CSI_UART, banner, strlen(banner));

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    wifi_init_sta();
    csi_init();
    hello_start();

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}