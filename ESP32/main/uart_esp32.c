#include "uart_esp32.h"
#include "driver/uart.h"
#include "esp_log.h"
#include <string.h>
#include "ws.h"

static const char *TAG = "UART";
#define BUF_SIZE 1024
static QueueHandle_t uart_queue;

int parse_3byte_le_to_int(uint8_t *ptr) {
    //return (ptr[2] << 16) | (ptr[1] << 8) | ptr[0];  // Little Endian 처리

    int value = (ptr[0] * 100) + (ptr[1] * 10) + ptr[2];
    return (value);
}
int temperature_offset = 0;

void uart_receive_task(void *arg)
{
    uint8_t data[BUF_SIZE];

    while (1)
    {
        int len = uart_read_bytes(UART_NUM, data, 31, pdMS_TO_TICKS(100));
        if (len == 31)
        {
            int type = data[0];

            int temperature     = parse_3byte_le_to_int(&data[1]);
            temperature += temperature_offset; 
            int humidity        = parse_3byte_le_to_int(&data[4]);
            int illuminance     = parse_3byte_le_to_int(&data[7]);
            int vibration       = parse_3byte_le_to_int(&data[10]);
            int motorSpeed      = parse_3byte_le_to_int(&data[13]);
            int isCarDoorOpen   = parse_3byte_le_to_int(&data[16]);
            int isACActive      = parse_3byte_le_to_int(&data[19]);
            int isSunroofOpen   = parse_3byte_le_to_int(&data[22]);
            int isActivate      = parse_3byte_le_to_int(&data[25]);
            int isDriving       = parse_3byte_le_to_int(&data[28]);
            
            ESP_LOGI(TAG, "UART 수신: isCarDoorOpen=%d isACActive=%d isSunroofOpen=%d isActivate=%d", isCarDoorOpen, isACActive, isSunroofOpen, isActivate);

            // 센서 JSON 전송
            char json_msg[256];
            snprintf(json_msg, sizeof(json_msg),
                "{\"type\":\"sensor\",\"payload\":{\"temperature\":%d,\"humidity\":%d,\"motorSpeedRaw\":%d,\"illuminance\":%d,\"vibration\":%d}}",
                temperature, humidity, motorSpeed, illuminance, vibration);

            if (esp_websocket_client_is_connected(client)) {
                esp_websocket_client_send_text(client, json_msg, strlen(json_msg), portMAX_DELAY);
                ESP_LOGI(TAG, "🌐 WebSocket 전송 (sensor): %s", json_msg);
            }

            char json_msg2[256];
            snprintf(json_msg2, sizeof(json_msg2),
                "{\"type\":\"carState\",\"payload\":{\"isCarDoorOpen\":%d,\"isSunroofOpen\":%d,\"isACActive\":%d,\"isAnomaly\":%d,\"isDriving\":%d,\"isActivate\":%d}}",
                isCarDoorOpen, isSunroofOpen, isACActive, 0, isDriving, isActivate);

            if (esp_websocket_client_is_connected(client)) {
                esp_websocket_client_send_text(client, json_msg2, strlen(json_msg2), portMAX_DELAY);
                ESP_LOGI(TAG, "🌐 WebSocket 전송 (carState): %s", json_msg2);
            }
        }
        else {
            // ESP_LOGW(TAG, "⚠️ 수신된 데이터 길이 오류: %d 바이트", len);
            continue;
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
