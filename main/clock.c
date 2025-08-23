#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_system.h"
#include "esp_log.h"
#include "8segDisplayLib.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include <string.h>

#include <stdbool.h>

#include "esp_sntp.h"
#include "esp_netif.h"
#include "esp_netif_sntp.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "credentials.h"

#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA_WPA2_PSK
#define ESP_WIFI_SAE_MODE WPA3_SAE_PWE_BOTH
#define EXAMPLE_H2E_IDENTIFIER ""



void singleDigitExample1(uint8_t segments[8], uint32_t delay){
  
  TickType_t delayProccesed = delay / portTICK_PERIOD_MS;

  initSingleDigit(segments);
  while(1){
    for(uint8_t i = 0; i < 10; i++){
      displaySingleNum(segments, i);
      vTaskDelay(delayProccesed);
    }
      
  }
}


/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

static const char *TAG = "wifi station";

static int s_retry_num = 0;


static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG,"connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

void wifi_init_sta(void)
{
    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .password = EXAMPLE_ESP_WIFI_PASS,
            /* Authmode threshold resets to WPA2 as default if password matches WPA2 standards (password len => 8).
             * If you want to connect the device to deprecated WEP/WPA networks, Please set the threshold value
             * to WIFI_AUTH_WEP/WIFI_AUTH_WPA_PSK and set the password with length and format matching to
             * WIFI_AUTH_WEP/WIFI_AUTH_WPA_PSK standards.
             */
            .threshold.authmode = ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD,
            .sae_pwe_h2e = ESP_WIFI_SAE_MODE,
            .sae_h2e_identifier = EXAMPLE_H2E_IDENTIFIER,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
                 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }
}

uint8_t counter = 0;
int minutes = 0;
int hours = 0;
bool x = true;


void countingTask(void *pvParameters) {
  while(1){
    x = !x;
    if(x == true){
      counter++;
    }
    if (counter > 59){
      counter = 0;
      minutes++;
      if (minutes == 60){
        hours++;
        minutes = 0;
        if (hours == 24){
          hours = 0;
        }
      }
    }
    vTaskDelay(500  / portTICK_PERIOD_MS);
  
  }
}



void initSNTP(){
  esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG("pool.ntp.org");
  
  esp_netif_sntp_init(&config);

}

void getTime(){
  initSNTP();

  int retry = 0;

  while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && retry < 10){
    ESP_LOGI("cajt", "cakam da se povezem ... (%d/10)", retry);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }

  time_t now = 0;
  struct tm timeinfo = {0};
  time(&now);
  localtime_r(&now, &timeinfo);
  ESP_LOGI("cajt", "current time = %s", asctime(&timeinfo));
}


void app_main(void){
  //                      A   B   C  D   E   F   G   DP  
  uint8_t segments1[8] = {27, 14, 1, 26, 33, 12, 25, 3};
  uint8_t segments2[8] = {4, 22, 19, 17, 5, 21, 16, 18};
  uint8_t digit1[2] = {13, 23};
  uint8_t digit2[2] = {2, 15};
  uint8_t nums1[2] = {1, 2};
  uint8_t nums2[2] = {3, 4};
  initMultipleSegments(segments1, digit1, 2);
  initMultipleSegments(segments2, digit2, 2);

  displaySingleNum(segments1, 0b1000000);
  displaySingleNum(segments2, 0b1000000);
  //vTaskDelay(1000 / portTICK_PERIOD_MS);  
  gpio_set_level(13, 0);
  gpio_set_level(23, 0);
  gpio_set_level(15, 0);
  gpio_set_level(2, 0);
  
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND){
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  if (CONFIG_LOG_MAXIMUM_LEVEL > CONFIG_LOG_DEFAULT_LEVEL) {
    esp_log_level_set("wifi", CONFIG_LOG_MAXIMUM_LEVEL);

  }
  wifi_init_sta();

  setenv("TZ", "CET-1CEST,M3.5.0/2,M10.5.0/3", 1);
  tzset();

  getTime();
  
  time_t now;
  struct tm timeinfo;
  time(&now);
  localtime_r(&now, &timeinfo);

  minutes = timeinfo.tm_min;
  hours = timeinfo.tm_hour;
  counter = timeinfo.tm_sec;

  xTaskCreatePinnedToCore(
    countingTask, //function
    "Counter", //name
    2048, //stack size in bytes 
    NULL, //paramaters
    1, //priority
    NULL, // task handle
    0  //core id
  );

  while(1){


    turnOnDigit(digit1[0]);
    turnOnDigit(digit2[0]);
    displaySingleNum(segments1, hours / 10); 
    displaySingleNum(segments2, minutes / 10);
    vTaskDelay(10 / portTICK_PERIOD_MS);
        
    turnOffDigit(digit1[0]);
    turnOffDigit(digit2[0]);
    
    turnOnDigit(digit1[1]);
    turnOnDigit(digit2[1]); 
    
    displaySingleNum(segments1, hours % 10);
    displaySingleNum(segments2, minutes % 10);
    
    if(x == true){
      gpio_set_level(3, 1);
    }

    vTaskDelay(10 / portTICK_PERIOD_MS);
    
    turnOffDigit(digit1[1]);
    turnOffDigit(digit2[1]);


    //displayNums(segments1, digit1, nums1 , 2);
    //displayNums(segments2, digit2, nums2, 2);
  
  }

}


/*void wifi_init(){
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());

  esp_netif_create_default_wifi_sta();
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));
  
  char* ssid = getSSID();
  char* pass = getPass();

  wifi_config_t wifi_config = {
    .sta = {
      .ssid = &ssid,
      .password = &pass,
    },
  };

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
  ESP_ERROR_CHECK(esp_wifi_start());

  ESP_ERROR_CHECK(esp_wifi_connect());
  vTaskDelay(pdMS_TO_TICKS(5000));


}


void initTimeServer() {
  esp_netif_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG("pool.ntp.org");
  ESP_ERROR_CHECK(esp_netif_sntp_init(&config));
  ESP_ERROR_CHECK(esp_netif_sntp_start());

  int retry = 0;
  while(esp_netif_sntp_sync_wait(pdMS_TO_TICKS(2000)) == ESP_ERR_TIMEOUT && retry < 10){
    ESP_LOGI("Pelin", "Waiting for the shit to sync,, lp (%d,10)", ++retry);
  }
  
}
*/

  







