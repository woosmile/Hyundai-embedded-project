#include "ble.h"
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_gatt_common_api.h"
#include <string.h>

#define GATTS_SERVICE_UUID   0x00FF
#define GATTS_CHAR_UUID      0xFF01
#define GATTS_NUM_HANDLE     4
#define DEVICE_NAME          "ESP32_BLE_DEVICE"
#define PROFILE_APP_IDX      0

static const char *TAG = "BLE";

// UUID 변수 선언
static const uint16_t primary_service_uuid = ESP_GATT_UUID_PRI_SERVICE;
static const uint16_t character_declaration_uuid = ESP_GATT_UUID_CHAR_DECLARE;
static const uint16_t gatts_service_uuid = GATTS_SERVICE_UUID;
static const uint16_t char_uuid = GATTS_CHAR_UUID;

static uint8_t char_value[20] = {0};
static uint16_t gatt_handle_table[GATTS_NUM_HANDLE];

static esp_gatt_char_prop_t property = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE;

// 🔧 광고 파라미터 추가
static esp_ble_adv_params_t adv_params = {
    .adv_int_min        = 0x20,
    .adv_int_max        = 0x40,
    .adv_type           = ADV_TYPE_IND,
    .own_addr_type      = BLE_ADDR_TYPE_PUBLIC,
    .channel_map        = ADV_CHNL_ALL,
    .adv_filter_policy  = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {
    switch (event) {
        case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
            esp_ble_gap_start_advertising(&adv_params);
            break;
        case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
            if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
                ESP_LOGE(TAG, "광고 시작 실패");
            } else {
                ESP_LOGI(TAG, "BLE 광고 시작");
            }
            break;
        default:
            break;
    }
}

// GATT 테이블
static esp_gatts_attr_db_t gatt_db[GATTS_NUM_HANDLE] = {
    // 서비스 선언
    [0] = {
        {ESP_GATT_AUTO_RSP},
        {
            ESP_UUID_LEN_16, (uint8_t *)&primary_service_uuid,
            ESP_GATT_PERM_READ,
            sizeof(uint16_t), sizeof(gatts_service_uuid),
            (uint8_t *)&gatts_service_uuid
        }
    },
    // 특성 선언
    [1] = {
        {ESP_GATT_AUTO_RSP},
        {
            ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid,
            ESP_GATT_PERM_READ,
            sizeof(uint8_t), sizeof(property),
            (uint8_t *)&property
        }
    },
    // 특성 값
    [2] = {
        {ESP_GATT_AUTO_RSP},
        {
            ESP_UUID_LEN_16, (uint8_t *)&char_uuid,
            ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
            sizeof(char_value), sizeof(char_value),
            char_value
        }
    }
};

static void gatts_profile_event_handler(esp_gatts_cb_event_t event,
                                        esp_gatt_if_t gatts_if,
                                        esp_ble_gatts_cb_param_t *param)
{
    switch (event) {
        case ESP_GATTS_REG_EVT: {
            ESP_LOGI(TAG, "서비스 등록 이벤트");
            esp_ble_gap_set_device_name(DEVICE_NAME);

            uint8_t service_uuid[2] = {
                (uint8_t)(GATTS_SERVICE_UUID & 0xFF),
                (uint8_t)((GATTS_SERVICE_UUID >> 8) & 0xFF)
            };

            esp_ble_adv_data_t adv_data = {
                .set_scan_rsp = false,
                .include_name = true,
                .include_txpower = true,
                .min_interval = 0x20,
                .max_interval = 0x40,
                .appearance = 0x00,
                .service_uuid_len = sizeof(service_uuid),
                .p_service_uuid = service_uuid,
                .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
            };

            esp_ble_gap_config_adv_data(&adv_data);
            esp_ble_gatts_create_attr_tab(gatt_db, gatts_if, GATTS_NUM_HANDLE, PROFILE_APP_IDX);
            break;
        }

        case ESP_GATTS_CREAT_ATTR_TAB_EVT:
            ESP_LOGI(TAG, "GATT 테이블 생성 완료");
            if (param->add_attr_tab.status != ESP_GATT_OK) {
                ESP_LOGE(TAG, "테이블 생성 실패");
                break;
            }
            if (param->add_attr_tab.num_handle == GATTS_NUM_HANDLE &&
                param->add_attr_tab.svc_inst_id == PROFILE_APP_IDX) {

                memcpy(gatt_handle_table, param->add_attr_tab.handles, sizeof(gatt_handle_table));
                esp_ble_gatts_start_service(gatt_handle_table[0]);
                ESP_LOGI(TAG, "서비스 시작됨");
            } else {
                ESP_LOGW(TAG, "잘못된 서비스 인덱스 또는 핸들 수");
            }
            break;

        case ESP_GATTS_CONNECT_EVT:
            ESP_LOGI(TAG, "✅ BLE 연결됨");
            break;

        case ESP_GATTS_DISCONNECT_EVT:
            ESP_LOGI(TAG, "❌ BLE 연결 끊김 → 광고 재시작");
            esp_ble_gap_start_advertising(&adv_params);
            break;

        case ESP_GATTS_WRITE_EVT:
            ESP_LOGI(TAG, "📥 클라이언트가 쓴 값: %.*s", param->write.len, param->write.value);
            break;

        case ESP_GATTS_READ_EVT:
            ESP_LOGI(TAG, "📤 클라이언트가 읽기 요청");
            break;

        default:
            break;
    }
}

void ble_init(const char *device_name)
{
    ESP_LOGI(TAG, "BLE 초기화 시작");

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_bt_controller_init(&bt_cfg));
    ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_BLE));
    ESP_ERROR_CHECK(esp_bluedroid_init());
    ESP_ERROR_CHECK(esp_bluedroid_enable());

    esp_ble_gap_register_callback(gap_event_handler);
    esp_ble_gatts_register_callback(gatts_profile_event_handler);
    esp_ble_gatts_app_register(PROFILE_APP_IDX);
}
