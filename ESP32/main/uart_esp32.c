#include "uart_esp32.h"
#include "driver/uart.h"
#include "esp_log.h"
#include <string.h>
#include "ws.h"

static const char *TAG = "UART";
#define BUF_SIZE 1024
static QueueHandle_t uart_queue;

void uart_receive_task(void *arg)
{
    uint8_t data[BUF_SIZE];

    while (1)
    {
        int len = uart_read_bytes(UART_NUM_1, data, BUF_SIZE - 1, pdMS_TO_TICKS(100));
        if (len > 0)
        {
            data[len] = '\0';  // 문자열 종료
            printf("UART 수신: %s\n", data);

            // 1. 길이 체크 (최소 12자리)
            if (len < 12) {
                ESP_LOGW(TAG, "데이터 길이 부족: %s", data);
                continue;
            }

            // 2. 파싱 (3자리씩 자름)
            char temp_str[4], humi_str[4], speed_str[4], lux_str[4];
            strncpy(temp_str, (char *)data, 3); temp_str[3] = '\0';
            strncpy(humi_str, (char *)data + 3, 3); humi_str[3] = '\0';
            strncpy(speed_str, (char *)data + 6, 3); speed_str[3] = '\0';
            strncpy(lux_str, (char *)data + 9, 3); lux_str[3] = '\0';

            int temperature = atoi(temp_str);
            int humidity = atoi(humi_str);
            int motorSpeed = atoi(speed_str);
            int illuminance = atoi(lux_str);

            // 3. JSON 문자열 생성
            char json_msg[256];
            snprintf(json_msg, sizeof(json_msg),
                "{\"type\":\"sensor\",\"payload\":{\"temperature\":%d,\"humidity\":%d,\"motorSpeed\":%d,\"illuminance\":%d}}",
                temperature, humidity, motorSpeed, illuminance);

             // 4. WebSocket으로 전송
            if (esp_websocket_client_is_connected(client)) {
                esp_websocket_client_send_text(client, json_msg, strlen(json_msg), portMAX_DELAY);
                ESP_LOGI(TAG, "🌐 WebSocket 전송: %s", json_msg);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void uart_init() {
    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    esp_err_t err = uart_driver_install(UART_NUM_1, BUF_SIZE * 2, 0, 0, NULL, 0);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "UART 드라이버 설치 실패: %s", esp_err_to_name(err));
    }
    uart_param_config(UART_NUM, &uart_config);
    uart_set_pin(UART_NUM, TX_PIN, RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    ESP_LOGI(TAG, "UART2 (TX: GPIO17, RX: GPIO16) 초기화 완료");
    xTaskCreate(uart_receive_task, "uart_receive_task", 4096, NULL, 10, NULL);
}

void uart_send_data(const char* data) {
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
