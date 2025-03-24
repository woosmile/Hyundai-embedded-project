#ifndef WS_H
#define WS_H
#include "esp_websocket_client.h"

extern esp_websocket_client_handle_t client;

void ws_start(void); // WebSocket 시작 함수

#endif
