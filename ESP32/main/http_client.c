#include "http_client.h"
#include "config.h"
#include "esp_log.h"
#include "esp_http_client.h"
#include <string.h>

static const char *TAG = "HTTP_CLIENT";

esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ERROR:
        ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
        break;
    case HTTP_EVENT_ON_CONNECTED:
        ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
        break;
    case HTTP_EVENT_HEADERS_SENT:  // 추가된 부분
        ESP_LOGI(TAG, "HTTP_EVENT_HEADERS_SENT");
        break;
    case HTTP_EVENT_ON_DATA:
        ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, length=%d", evt->data_len);
        printf("%.*s\n", evt->data_len, (char *)evt->data);
        break;
    case HTTP_EVENT_ON_FINISH:
        ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
        break;
    case HTTP_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
        break;
    default:
        ESP_LOGW(TAG, "Unhandled HTTP event: %d", evt->event_id);
        break;
    }
    
    return ESP_OK;
}

void http_get_request()
{
    esp_http_client_config_t config = {
        .url = "https://hyundai-app-cf6af1f123b1.herokuapp.com/api/sensor",
        .event_handler = _http_event_handler,
        .cert_pem = HTTPS_CERT,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK)
    {
        ESP_LOGI(TAG, "HTTPS GET 요청 성공, 응답 코드: %d", esp_http_client_get_status_code(client));
    }
    else
    {
        ESP_LOGE(TAG, "HTTPS GET 요청 실패: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
}

void http_post_request(const char *json_payload)
{
    esp_http_client_config_t config = {
        .url = "https://hyundai-app-cf6af1f123b1.herokuapp.com/api/sensor",
        .event_handler = _http_event_handler,
        .cert_pem = HTTPS_CERT,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);

    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, json_payload, strlen(json_payload));

    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK)
    {
        ESP_LOGI(TAG, "HTTPS POST 요청 성공, 응답 코드: %d", esp_http_client_get_status_code(client));
    }
    else
    {
        ESP_LOGE(TAG, "HTTPS POST 요청 실패: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
}