#include "ws.h"
#include "uart_esp32.h"
#include "esp_log.h"
#include "esp_websocket_client.h"
#include <string.h>
#include "config.h"
static const char *TAG = "websocket";
esp_websocket_client_handle_t client = NULL;

typedef struct {
    bool isCarDoorOpen;
    bool isSunroofOpen;
    bool isACActive;
    bool isAnomaly;
} CarState;

CarState car_state = {
    .isCarDoorOpen = false,
    .isSunroofOpen = false,
    .isACActive = false,
    .isAnomaly = false
};


void update_car_state_and_notify_ws()
{
    if (!esp_websocket_client_is_connected(client)) return;

    char json_msg[256];
    snprintf(json_msg, sizeof(json_msg),
             "{\"type\":\"carState\",\"payload\":{"
             "\"isCarDoorOpen\":%s,"
             "\"isSunroofOpen\":%s,"
             "\"isACActive\":%s,"
             "\"isDriving\":%s}}",
             car_state.isCarDoorOpen ? "true" : "false",
             car_state.isSunroofOpen ? "true" : "false",
             car_state.isACActive ? "true" : "false",
             car_state.isAnomaly ? "true" : "false");

    esp_websocket_client_send_text(client, json_msg, strlen(json_msg), portMAX_DELAY);
    ESP_LOGI("CAR_STATE", "📤 상태 전송: %s", json_msg);
}

static void websocket_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_websocket_event_data_t *data = (esp_websocket_event_data_t *)event_data;

    switch (event_id)
    {
    case WEBSOCKET_EVENT_CONNECTED:
        ESP_LOGI(TAG, "WebSocket connected");
        const char *register_msg = "{\"type\": \"register\", \"role\": \"esp32\"}";
        esp_websocket_client_send_text(client, register_msg, strlen(register_msg), portMAX_DELAY);
        break;

    case WEBSOCKET_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "WebSocket disconnected");
        break;

    case WEBSOCKET_EVENT_DATA:
        if (data->data_len > 0) {
            ESP_LOGI(TAG, "📥 명령 수신: %.*s", data->data_len, (char *)data->data_ptr);

            char cmd_buf[64] = {0};
            int len = data->data_len > sizeof(cmd_buf) - 1 ? sizeof(cmd_buf) - 1 : data->data_len;
            memcpy(cmd_buf, data->data_ptr, len);
            cmd_buf[len] = '\0';  
    
            uart_send_data(cmd_buf);    

            if (strcmp(cmd_buf, "toggle_door") == 0) {
                car_state.isCarDoorOpen = !car_state.isCarDoorOpen;
            } else if (strcmp(cmd_buf, "toggle_sunroof") == 0) {
                car_state.isSunroofOpen = !car_state.isSunroofOpen;
            } else if (strcmp(cmd_buf, "toggle_ac") == 0) {
                car_state.isACActive = !car_state.isACActive;
            } 

            // 웹으로 상태 전송
            update_car_state_and_notify_ws();   
        } 
        break;

    case WEBSOCKET_EVENT_ERROR:
        ESP_LOGE(TAG, "WebSocket error");
        break;
    }
}

void ws_start(void)
{
    esp_websocket_client_config_t websocket_cfg = {
        .uri = "wss://hyundai-app-cf6af1f123b1.herokuapp.com/ws", 
        .cert_pem = HTTPS_CERT,  
    };
    
    client = esp_websocket_client_init(&websocket_cfg);
    esp_websocket_register_events(client, WEBSOCKET_EVENT_ANY, websocket_event_handler, (void *)client);
    esp_websocket_client_start(client);
}

