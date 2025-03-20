#include <stdio.h>
#include "wifi.h"
#include "uart_esp32.h"
#include "http_client.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void)
{
    printf("ESP32 시작\n");

    // Wi-Fi 연결
    if (wifi_init_sta() == ESP_OK)
    {
        printf("Wi-Fi 연결 성공!\n");

        // HTTPS 요청 전송
        vTaskDelay(pdMS_TO_TICKS(3000));
        http_get_request();
    }
    else
    {
        printf("Wi-Fi 연결 실패!\n");
    }

    // UART 초기화
    uart_init();  // UART 초기화

    volatile char *test_msg = "Hello ";
    volatile char rx_buffer[BUF_SIZE];

    while (1)
    {
        // // UART 데이터 송신
        // uart_send(test_msg);

        // // UART 데이터 수신
        // uart_receive_data();

        vTaskDelay(pdMS_TO_TICKS(2000)); // 2초 대기
    }
}
