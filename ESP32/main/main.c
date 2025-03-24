#include <stdio.h>
#include "wifi.h"
#include "ble.h"
#include "uart_esp32.h"
#include "http_client.h"
#include "ws.h"  
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


//센서시뮬레이션
void sensor_loop_task(void *pvParameters)
{
    char json_data[128];
    int counter = 1;

    while (1)
    {
        // 1~60까지 센서값 시뮬레이션
        int value = counter;

        snprintf(json_data, sizeof(json_data),
        "{\"type\":\"sensor\",\"payload\":{\"temperature\":%d, \"humidity\":%d, \"motorSpeed\":%d, \"illuminance\":%d}}",
        value, value + 10, value * 20, value * 5);

        esp_websocket_client_send_text(client, json_data, strlen(json_data), portMAX_DELAY);

        counter++;
        if (counter > 60)
            counter = 1;

        vTaskDelay(pdMS_TO_TICKS(1000)); // 1초 대기
    }
}


void app_main(void)
{
    printf("ESP32 시작\n");

    // Wi-Fi 연결
    if (wifi_init_sta() == ESP_OK)
    {
        printf("Wi-Fi 연결 성공!\n");

        // Wi-Fi 연결 완료 후 WebSocket 시작
        vTaskDelay(pdMS_TO_TICKS(3000));
        ws_start();  // WebSocket 클라이언트 시작
    }
    else
    {
        printf("Wi-Fi 연결 실패!\n");
    }



    while (!esp_websocket_client_is_connected(client)) {
        vTaskDelay(pdMS_TO_TICKS(500));
        printf("⏳ WebSocket 연결 대기 중...\n");
    }
    
    // UART 초기화
    uart_init();

    // BLE 초기화 (필요 시 주석 해제)
    // ble_init("ESP32_BLE_DEVICE");

    //자동 센서 정보 생성 시작
    BaseType_t result = xTaskCreate(&sensor_loop_task, "sensor_loop_task", 4096, NULL, 5, NULL);
    if (result == pdPASS) {
        printf("센서 Task 시작됨\n");
    } else {
        printf("센서 Task 생성 실패!\n");
    }

}
