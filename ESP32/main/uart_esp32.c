#include "uart_esp32.h"
#include "driver/uart.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "UART";

void uart_init() {
    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    uart_driver_install(UART_NUM, BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM, &uart_config);
    uart_set_pin(UART_NUM, TX_PIN, RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    ESP_LOGI(TAG, "UART2 (TX: GPIO17, RX: GPIO16) 초기화 완료");
}

void uart_send(const char* data) {
    uart_write_bytes(UART_NUM, data, strlen(data));
    ESP_LOGI(TAG, "전송: %s", data);
}

void uart_receive_data() {
    uint8_t data[128];
    int len = uart_read_bytes(UART_NUM, data, sizeof(data), 100 / portTICK_PERIOD_MS);
    if (len > 0) {
        data[len] = '\0';
        ESP_LOGI(TAG, "수신: %s", data);
    }
}
