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
    bool isDriving;
    bool isActivate;
} CarState;

CarState car_state = {
    .isCarDoorOpen = false,
    .isSunroofOpen = false,
    .isACActive = false,
    .isAnomaly = false,
    .isDriving = false,
    .isActivate = false,
};


// void update_car_state_and_notify_ws()
// {
//     if (!esp_websocket_client_is_connected(client)) return;

//     char json_msg[256];
//     snprintf(json_msg, sizeof(json_msg),
//              "{\"type\":\"carState\",\"payload\":{"
//              "\"isCarDoorOpen\":%s,"
//              "\"isSunroofOpen\":%s,"
//              "\"isACActive\":%s,"
//              "\"isAnomaly\":%s,"
//              "\"isDriving\":%s}}",
//              car_state.isCarDoorOpen ? "true" : "false",
//              car_state.isSunroofOpen ? "true" : "false",
//              car_state.isACActive ? "true" : "false",
//              car_state.isAnomaly ? "true" : "false",
//              car_state.isDriving ? "true" : "false");

//     esp_websocket_client_send_text(client, json_msg, strlen(json_msg), portMAX_DELAY);
//     ESP_LOGI("CAR_STATE", "📤 상태 전송: %s", json_msg);
// }

void send_car_state_as_uart_code(uint8_t state_code) {
    char msg[8];
    snprintf(msg, sizeof(msg), "%d", state_code);
    uart_send_data(msg);

    ESP_LOGI(TAG, "📤 UART 상태코드 전송: %d", state_code);
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

            bool valid_command = false;
            uint8_t state_code;
            char cmd_buf[64] = {0};
            int len = data->data_len > sizeof(cmd_buf) - 1 ? sizeof(cmd_buf) - 1 : data->data_len;
            memcpy(cmd_buf, data->data_ptr, len);
            cmd_buf[len] = '\0';   

            //차량 상태 변경
            if (strcmp(cmd_buf, "toggle_start") == 0) {
                car_state.isActivate = !car_state.isActivate;
                state_code = car_state.isActivate ? 1 : 0;
                valid_command = true;
            } else if (strcmp(cmd_buf, "toggle_sunroof") == 0) {
                car_state.isSunroofOpen = !car_state.isSunroofOpen;
                state_code = car_state.isSunroofOpen ? 3 : 2;
                valid_command = true;
            } else if (strcmp(cmd_buf, "toggle_ac") == 0) {
                car_state.isACActive = !car_state.isACActive;
                state_code = car_state.isACActive ? 5 : 4;
                valid_command = true;
            } else if (strcmp(cmd_buf, "toggle_drive") == 0) {
                car_state.isDriving = !car_state.isDriving;
                state_code = car_state.isDriving ? 7 : 6;
                valid_command = true;
            } else if (strcmp(cmd_buf, "toggle_door") == 0) {
                car_state.isCarDoorOpen = !car_state.isCarDoorOpen;
                state_code = car_state.isCarDoorOpen ? 9 : 8;
                valid_command = true;
            } 
            
            //uart 전송
            if(valid_command){
                send_car_state_as_uart_code(state_code);
            }
            else{
                ESP_LOGW(TAG, "wrong command");
            }
            
            // 웹으로 상태 전송
            // update_car_state_and_notify_ws();   
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

