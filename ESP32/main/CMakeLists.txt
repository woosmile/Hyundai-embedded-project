idf_component_register(SRCS 
    "main.c"
    "ws.c"
    "wifi.c"
    "uart_esp32.c"
    "http_client.c"
    "ble.c"
    INCLUDE_DIRS "."
    REQUIRES 
        esp_websocket_client 
        nvs_flash 
        esp_http_client 
        esp_wifi
        driver  
        bt
    )

