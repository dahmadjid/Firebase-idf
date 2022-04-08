
#include <iostream>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_http_client.h"
#include "esp_log.h"
#include "esp_tls.h"
#include "jsoncpp/value.h"
#include "jsoncpp/json.h"

#include "esp_firebase.h"


#define HTTP_TAG "HTTP_CLIENT"
#define RTDB_TAG "RTDB"

extern const char cert_start[] asm("_binary_gtsr1_pem_start");
extern const char cert_end[]   asm("_binary_gtsr1_pem_end");

using namespace ESPFirebase;

static esp_err_t http_event_handler(esp_http_client_event_t *evt)
{

    static int output_len; 
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGD(HTTP_TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGI(HTTP_TAG, "HTTP_EVENT_ON_CONNECTED");
            output_len = 0;
            memset(evt->user_data, 0, HTTP_RECV_BUFFER_SIZE);
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

// TODO: protect this function from breaking 
void Firebase::firebaseClientInit(void)
{   
    esp_http_client_config_t config = {0};
    config.url = Firebase::base_database_url.c_str();
    config.event_handler = http_event_handler;
    config.cert_pem = Firebase::https_certificate;
    config.user_data = Firebase::local_response_buffer;
    config.buffer_size_tx = 4096;
    config.buffer_size = HTTP_RECV_BUFFER_SIZE;
    Firebase::client = esp_http_client_init(&config);
    ESP_LOGI(HTTP_TAG, "HTTP Client Initialized");

}
esp_err_t Firebase::setHeader(const char* header, const char* value)
{
    return esp_http_client_set_header(Firebase::client, header, value);
}

http_ret_t Firebase::performClient(const char* url, esp_http_client_method_t method, std::string post_field)
{
    ESP_ERROR_CHECK(esp_http_client_set_url(Firebase::client, url));
    ESP_ERROR_CHECK(esp_http_client_set_method(Firebase::client, method));
    ESP_ERROR_CHECK(esp_http_client_set_post_field(Firebase::client, post_field.c_str(), post_field.length()));
    esp_err_t err = esp_http_client_perform(Firebase::client);
    int status_code = esp_http_client_get_status_code(Firebase::client);
    if (err != ESP_OK || status_code != 200)
    {
        ESP_LOGE(HTTP_TAG, "Error while performing requst err=%d | status_code=%d", (int)err, status_code);
        ESP_LOGE(HTTP_TAG, "request: url=%s | method=%d | post_field=%s", url, method, post_field.c_str());
        ESP_LOGE(HTTP_TAG, "response=\n%s", local_response_buffer);
    }
    return {err, status_code};
}

esp_err_t Firebase::getRefreshToken(const user_account_t& account, bool register_account)
{


    http_ret_t http_ret;
    
    std::string account_json = R"({"email":")";
    account_json += account.user_email; 
    account_json += + R"(", "password":")"; 
    account_json += account.user_password;
    account_json += R"(", "returnSecureToken": true})"; 

    Firebase::setHeader("content-type", "application/json");
    if (register_account)
    {
        http_ret = Firebase::performClient(Firebase::register_url.c_str(), HTTP_METHOD_POST, account_json);
    }
    else
    {
        http_ret = Firebase::performClient(Firebase::login_url.c_str(), HTTP_METHOD_POST, account_json);
    }

    if (http_ret.err == ESP_OK && http_ret.status_code == 200)
    {
        std::cout << Firebase::local_response_buffer << '\n';
        const char* begin = Firebase::local_response_buffer;
        const char* end = begin + strlen(Firebase::local_response_buffer);
        Json::Reader reader;
        Json::Value data;
        reader.parse(begin, end, data, false);
        Firebase::refresh_token = data["refreshToken"].asString();

        ESP_LOGI(HTTP_TAG, "Refresh Token=%s", Firebase::refresh_token.c_str());
        return ESP_OK;
    }
    else 
    {
        return ESP_FAIL;
    }
}
esp_err_t Firebase::getAuthToken()
{
    http_ret_t http_ret;

    std::string token_post_data = R"({"grant_type": "refresh_token", "refresh_token":")";
    token_post_data+= Firebase::refresh_token + "\"}";


    Firebase::setHeader("content-type", "application/json");
    http_ret = Firebase::performClient(Firebase::auth_url.c_str(), HTTP_METHOD_POST, token_post_data);
    if (http_ret.err == ESP_OK && http_ret.status_code == 200)
    {
        const char* begin = Firebase::local_response_buffer;
        const char* end = begin + strlen(Firebase::local_response_buffer);
        Json::Reader reader;
        Json::Value data;
        reader.parse(begin, end, data, false);
        Firebase::auth_token = data["access_token"].asString();

        ESP_LOGI(HTTP_TAG, "Auth Token=%s", Firebase::auth_token.c_str());

        return ESP_OK;
    }
    else 
    {
        return ESP_FAIL;
    }

    
}



Firebase::Firebase(const config_t& config)
    : https_certificate(cert_start), api_key(config.api_key), base_database_url(config.database_url)
{
    
    Firebase::local_response_buffer = new char[HTTP_RECV_BUFFER_SIZE];
    Firebase::register_url += Firebase::api_key; 
    Firebase::login_url += Firebase::api_key;
    Firebase::auth_url += Firebase::api_key;
    firebaseClientInit();
}

Firebase::~Firebase()
{
    delete[] Firebase::local_response_buffer;
    esp_http_client_cleanup(Firebase::client);
}

esp_err_t Firebase::registerUserAccount(const user_account_t& account)
{
    esp_err_t err = Firebase::getRefreshToken(account, true);
    if (err != ESP_OK)
    {
        ESP_LOGE(HTTP_TAG, "Failed to get refresh token");
        return ESP_FAIL;
    }
    err = Firebase::getAuthToken();
    if (err != ESP_OK)
    {
        ESP_LOGE(HTTP_TAG, "Failed to get auth token");
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t Firebase::loginUserAccount(const user_account_t& account)
{
    esp_err_t err = Firebase::getRefreshToken(account, false);
    if (err != ESP_OK)
    {
        ESP_LOGE(HTTP_TAG, "Failed to get refresh token");
        return ESP_FAIL;
    }
    err = Firebase::getAuthToken();
    if (err != ESP_OK)
    {
        ESP_LOGE(HTTP_TAG, "Failed to get auth token");
        return ESP_FAIL;
    }
    return ESP_OK;
}

Json::Value Firebase::getData(const char* path)
{
    
    std::string url = Firebase::base_database_url;
    url += path;
    url += ".json?auth=" + Firebase::auth_token;

    Firebase::setHeader("content-type", "application/json");
    http_ret_t http_ret = Firebase::performClient(url.c_str(), HTTP_METHOD_GET, "");
    if (http_ret.err == ESP_OK && http_ret.status_code == 200)
    {
        std::cout << Firebase::local_response_buffer << std::endl;
        const char* begin = Firebase::local_response_buffer;
        const char* end = begin + strlen(Firebase::local_response_buffer);

        Json::Reader reader;
        Json::Value data;

        reader.parse(begin, end, data, false);

        ESP_LOGI(RTDB_TAG, "Data with path=%s acquired", path);
        return data;
    }
    else
    {   
        ESP_LOGE(RTDB_TAG, "Error while getting data at path %s| esp_err_t=%d | status_code=%d", path, (int)http_ret.err, http_ret.status_code);
        return Json::Value();
    }
}

// esp_err_t putData(const char* path, const std::string json_data)
// {
//     esp_http_client_set_url(Firebase::client, url.c_str());
//     esp_http_client_set_header(Firebase::client, "content-type", "application/json");
//     esp_http_client_set_post_field(Firebase::client, json_data.c_str(), json_data.length());
//     esp_http_client_set_method(Firebase::client, HTTP_METHOD_PUT);
// }