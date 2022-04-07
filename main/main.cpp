
#include <iostream>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"

#include "esp_http_client.h"

#include "jsoncpp/value.h"
#include "jsoncpp/json.h"

#include "wifi_utils.h"

#define SSID "SSID"
#define PASSWORD "PASSWORD"
#define MAX_HTTP_OUTPUT_BUFFER 51200
#define HTTP_TAG "HTTP_CLIENT"

extern const char cert_start[] asm("_binary_gtsr1_pem_start");
extern const char cert_end[]   asm("_binary_gtsr1_pem_end");

char local_response_buffer[MAX_HTTP_OUTPUT_BUFFER] = {0};

esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    static char *output_buffer;  // Buffer to store response of http request from event handler
          // Stores number of bytes read
    static int output_len; 
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGD(HTTP_TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGI(HTTP_TAG, "HTTP_EVENT_ON_CONNECTED");
            output_len = 0;
            memset(evt->user_data, 0, MAX_HTTP_OUTPUT_BUFFER);
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGD(HTTP_TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGD(HTTP_TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGI(HTTP_TAG, "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGI(HTTP_TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            memcpy(evt->user_data + output_len, evt->data, evt->data_len);
            output_len += evt->data_len;
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(HTTP_TAG, "HTTP_EVENT_DISCONNECTED");
            break;
    }
    return ESP_OK;
}


extern "C" void app_main(void)
{
    wifiInit(SSID, PASSWORD);  // BLOCKING

    esp_http_client_config_t config = {0};
    // config.url = "https://www.googleapis.com/identitytoolkit/v3/relyingparty/verifyPassword?key="API_KEY;
    config.url = "https://www.googleapis.com/identitytoolkit/v3/relyingparty/verifyPassword?key=";
    //config.url = "https://fuel-consumption-742fd-default-rtdb.europe-west1.firebasedatabase.app/Base%20Camp%200.json?auth=";
    config.method = HTTP_METHOD_POST;
    config.event_handler = _http_event_handler;
    config.cert_pem = cert_start;
    config.user_data = local_response_buffer;
    
    esp_http_client_handle_t client = esp_http_client_init(&config);
    const std::string account = R"({"email": "test@gmail.com", "password": "testpass", "returnSecureToken": true})";  

    esp_http_client_set_header(client, "content-type", "application/json");
    esp_http_client_set_post_field(client, account.c_str(), account.length());
    esp_err_t err = esp_http_client_perform(client);
    
    esp_http_client_cleanup(client);    
    if (err == ESP_OK) 
    {
        ESP_LOGI(HTTP_TAG, "HTTPS Status = %d, content_length = %d", esp_http_client_get_status_code(client), esp_http_client_get_content_length(client));
    } 
    else 
    {
        ESP_LOGE(HTTP_TAG, "Error perform http request %s", esp_err_to_name(err));
    }
    std::string response = std::string(local_response_buffer);
    Json::Reader reader;
    Json::Value root;
    reader.parse(response, root);
    std::string token = root["refreshToken"].asString();
    std::cout << "refresh token: " <<  token << '\n';



    config = {0};
    // config.url = "https://www.googleapis.com/identitytoolkit/v3/relyingparty/verifyPassword?key="API_KEY;
    config.url = "https://securetoken.googleapis.com/v1/token?key=";
    //config.url = "https://fuel-consumption-742fd-default-rtdb.europe-west1.firebasedatabase.app/Base%20Camp%200.json?auth=";
    config.method = HTTP_METHOD_POST;
    config.event_handler = _http_event_handler;
    config.cert_pem = cert_start;
    config.user_data = local_response_buffer;
    
    client = esp_http_client_init(&config);
    std::string token_post_data = R"({"grant_type": "refresh_token", "refresh_token":")";
    token_post_data+= token + "\"}";

    esp_http_client_set_header(client, "content-type", "application/json");
    esp_http_client_set_post_field(client, token_post_data.c_str(), token_post_data.length());
    err = esp_http_client_perform(client);
    esp_http_client_cleanup(client);    
    if (err == ESP_OK) 
    {
        ESP_LOGI(HTTP_TAG, "HTTPS Status = %d, content_length = %d", esp_http_client_get_status_code(client), esp_http_client_get_content_length(client));
    } 
    else 
    {
        ESP_LOGE(HTTP_TAG, "Error perform http request %s", esp_err_to_name(err));
    }
    // std::cout << "request payload: " << token_post_data << '\n';
    response = std::string(local_response_buffer);
    reader.parse(response, root);
    std::cout << "auth: " << root["access_token"].asString() << '\n';


    
    std::string url = "https://fuel-consumption-742fd-default-rtdb.europe-west1.firebasedatabase.app/test.json?auth=";
    url+=root["access_token"].asString();
    config.url = url.c_str();
    
    config.method = HTTP_METHOD_GET;
    config.event_handler = _http_event_handler;
    config.cert_pem = cert_start;
    config.user_data = local_response_buffer;
    config.buffer_size = 51200;
    config.buffer_size_tx = 25600;
    client = esp_http_client_init(&config);
    esp_http_client_set_header(client, "content-type", "application/json");
    err = esp_http_client_perform(client);
    esp_http_client_cleanup(client);    
    if (err == ESP_OK) 
    {
        ESP_LOGI(HTTP_TAG, "HTTPS Status = %d, content_length = %d", esp_http_client_get_status_code(client), esp_http_client_get_content_length(client));
    } 
    else 
    {
        ESP_LOGE(HTTP_TAG, "Error perform http request %s", esp_err_to_name(err));
    }
    response = std::string(local_response_buffer);
    std::cout << url <<"response: " << response << '\n';

    
}