#include "esp_netif.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "wifi.h"
#include "nvs_flash.h"
#include <string.h>

#define INPUT_ESP_WIFI_SSID CONFIG_ESP_WIFI_SSID
#define INPUT_ESP_WIFI_PASS CONFIG_ESP_WIFI_PASS
#define MAX_RETRY_CONNECT_NUM 5

static const char *TAG = "WifiModule";

struct Wifi
{
    char* ssid;
    char* pass;
    int retry_num;
};

Wifi_handle_t Wifi_create()
{
    Wifi_handle_t instance = (Wifi_handle_t) malloc(1 * sizeof(struct Wifi));
    return instance;
}

void Wifi_destroy(Wifi_handle_t me)
{
    free(me);
}

static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    Wifi_handle_t pt = (Wifi_handle_t) arg;
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        // if ( pt->retry_num < MAX_RETRY_CONNECT_NUM)
        // {
        //     esp_wifi_connect();
        //     pt->retry_num++;
        //     ESP_LOGI(TAG, "retry connected after Disconnected");
        // }
        esp_wifi_connect();
    }
    else if(event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t* event = (ip_event_got_ip_t *)event_data;
        pt->retry_num = 0;
        ESP_LOGI(TAG, "Received IP packet!");
    }
}

static void event_handler_wifi_setup(Wifi_handle_t me)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_event_handler_instance_t instance_wifi_id;
    esp_event_handler_instance_t instance_got_up_ip;

    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        WIFI_EVENT, 
        ESP_EVENT_ANY_ID, 
        &wifi_event_handler, 
        me, 
        &instance_wifi_id 
    ));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        IP_EVENT,
        IP_EVENT_STA_GOT_IP,
        &wifi_event_handler,
        me,
        &instance_got_up_ip
    ));
}

void Wifi_init(Wifi_handle_t me)
{
    ESP_ERROR_CHECK(esp_netif_init());
    
    event_handler_wifi_setup(me);

    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg_wifi = WIFI_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK(esp_wifi_init(&cfg_wifi));

    me->ssid = INPUT_ESP_WIFI_SSID;
    me->pass = INPUT_ESP_WIFI_PASS;

    wifi_config_t wifi_sta_config = 
    {
        .sta = {
            .ssid = INPUT_ESP_WIFI_SSID,
            .password = INPUT_ESP_WIFI_PASS,
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_sta_config) );
    ESP_ERROR_CHECK(esp_wifi_start());
}

