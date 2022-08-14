
#include <iostream>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "esp_tls.h"

#include "app.h"



#include "jsoncpp/value.h"
#include "jsoncpp/json.h"

// #include "nvs.h"
// #include "nvs_flash.h"
#define NVS_TAG "NVS"


#define HTTP_TAG "HTTP_CLIENT"
#define FIREBASE_APP_TAG "FirebaseApp"

extern const char cert_start[] asm("_binary_gtsr1_pem_start");
extern const char cert_end[]   asm("_binary_gtsr1_pem_end");



static int output_len = 0; 
static esp_err_t http_event_handler(esp_http_client_event_t *evt)
{

    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGD(HTTP_TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGD(HTTP_TAG, "HTTP_EVENT_ON_CONNECTED");
            memset(evt->user_data, 0, HTTP_RECV_BUFFER_SIZE);
            output_len = 0;
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGD(HTTP_TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGD(HTTP_TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD(HTTP_TAG, "HTTP_EVENT_ON_FINISH");
            output_len = 0;
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGD(HTTP_TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            memcpy(evt->user_data + output_len, evt->data, evt->data_len);
            output_len += evt->data_len;
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGD(HTTP_TAG, "HTTP_EVENT_DISCONNECTED");
            break;
    }
    return ESP_OK;
}

namespace ESPFirebase {

// TODO: protect this function from breaking 
void FirebaseApp::firebaseClientInit(void)
{   
    esp_http_client_config_t config = {0};
    config.url = "https://google.com";    // you have to set this as https link of some sort so that it can init properly, you cant leave it empty
    config.event_handler = http_event_handler;
    config.cert_pem = FirebaseApp::https_certificate;
    config.user_data = FirebaseApp::local_response_buffer;
    config.buffer_size_tx = 4096;
    config.buffer_size = HTTP_RECV_BUFFER_SIZE;
    FirebaseApp::client = esp_http_client_init(&config);
    ESP_LOGD(FIREBASE_APP_TAG, "HTTP Client Initialized");

}


esp_err_t FirebaseApp::setHeader(const char* header, const char* value)
{
    return esp_http_client_set_header(FirebaseApp::client, header, value);
}

http_ret_t FirebaseApp::performRequest(const char* url, esp_http_client_method_t method, std::string post_field)
{
    ESP_ERROR_CHECK(esp_http_client_set_url(FirebaseApp::client, url));
    ESP_ERROR_CHECK(esp_http_client_set_method(FirebaseApp::client, method));
    ESP_ERROR_CHECK(esp_http_client_set_post_field(FirebaseApp::client, post_field.c_str(), post_field.length()));
    esp_err_t err = esp_http_client_perform(FirebaseApp::client);
    int status_code = esp_http_client_get_status_code(FirebaseApp::client);
    if (err != ESP_OK || status_code != 200)
    {
        ESP_LOGE(FIREBASE_APP_TAG, "Error while performing request esp_err_t code=0x%x | status_code=%d", (int)err, status_code);
        ESP_LOGE(FIREBASE_APP_TAG, "request: url=%s \nmethod=%d \npost_field=%s", url, method, post_field.c_str());
        ESP_LOGE(FIREBASE_APP_TAG, "response=\n%s", local_response_buffer);
    }
    return {err, status_code};
}

void FirebaseApp::clearHTTPBuffer(void)
{   
    memset(FirebaseApp::local_response_buffer, 0, HTTP_RECV_BUFFER_SIZE);
    output_len = 0;
}
esp_err_t FirebaseApp::getRefreshToken(bool register_account)
{


    http_ret_t http_ret;
    
    std::string account_json = R"({"email":")";
    account_json += FirebaseApp::user_account.user_email; 
    account_json += + R"(", "password":")"; 
    account_json += FirebaseApp::user_account.user_password;
    account_json += R"(", "returnSecureToken": true})"; 

    FirebaseApp::setHeader("content-type", "application/json");
    if (register_account)
    {
        http_ret = FirebaseApp::performRequest(FirebaseApp::register_url.c_str(), HTTP_METHOD_POST, account_json);
    }
    else
    {
        http_ret = FirebaseApp::performRequest(FirebaseApp::login_url.c_str(), HTTP_METHOD_POST, account_json);
    }

    if (http_ret.err == ESP_OK && http_ret.status_code == 200)
    {
        // std::cout << FirebaseApp::local_response_buffer << '\n';
        const char* begin = FirebaseApp::local_response_buffer;
        const char* end = begin + strlen(FirebaseApp::local_response_buffer);
        Json::Reader reader;
        Json::Value data;
        reader.parse(begin, end, data, false);
        FirebaseApp::refresh_token = data["refreshToken"].asString();

        ESP_LOGD(FIREBASE_APP_TAG, "Refresh Token=%s", FirebaseApp::refresh_token.c_str());
        return ESP_OK;
    }
    else 
    {
        return ESP_FAIL;
    }
}
esp_err_t FirebaseApp::getAuthToken()
{
    http_ret_t http_ret;

    std::string token_post_data = R"({"grant_type": "refresh_token", "refresh_token":")";
    token_post_data+= FirebaseApp::refresh_token + "\"}";


    FirebaseApp::setHeader("content-type", "application/json");
    http_ret = FirebaseApp::performRequest(FirebaseApp::auth_url.c_str(), HTTP_METHOD_POST, token_post_data);
    if (http_ret.err == ESP_OK && http_ret.status_code == 200)
    {
        const char* begin = FirebaseApp::local_response_buffer;
        const char* end = begin + strlen(FirebaseApp::local_response_buffer);
        Json::Reader reader;
        Json::Value data;
        reader.parse(begin, end, data, false);
        FirebaseApp::auth_token = data["access_token"].asString();

        ESP_LOGD(FIREBASE_APP_TAG, "Auth Token=%s", FirebaseApp::auth_token.c_str());

        return ESP_OK;
    }
    else 
    {
        return ESP_FAIL;
    }

    
}

// esp_err_t FirebaseApp::nvsSaveTokens() // useless until expire time added
// {
//     nvs_handle_t my_handle;
//     esp_err_t err;
//     err = nvs_open("storage", NVS_READWRITE, &my_handle);
//     err = nvs_set_str(my_handle, "refresh", FirebaseApp::refresh_token.c_str());
//     err = nvs_set_str(my_handle, "auth", FirebaseApp::auth_token.c_str());
//     err = nvs_commit(my_handle);
//     nvs_close(my_handle);
//     ESP_LOGD(NVS_TAG, "Tokens saved");
//     return err;

// }
// esp_err_t FirebaseApp::nvsReadTokens() // useless until expire time added
// {
//     nvs_handle_t my_handle;
//     esp_err_t err;
//     int refresh_len, auth_len = 0;
//     char refresh[500] = {0};
//     char auth[500] = {0};
//     err = nvs_open("storage", NVS_READWRITE, &my_handle);

//     // err = nvs_set_i16(my_handle, "refresh_len", &refresh_len);
//     // err = nvs_set_i16(my_handle, "auth_len", &auth_len);

//     err = nvs_get_str(&my_handle, "refresh", &refresh_temp, &refresh_len);
//     err = nvs_get_str(&my_handle, "auth", &auth_temp, &auth_len);
//     err = nvs_commit(my_handle);
//     nvs_close(my_handle);

//     FirebaseApp::refresh_token = std::string(refresh_temp);
//     FirebaseApp::auth_token = std::string(auth_temp);

//     ESP_LOGD(NVS_TAG, "Tokens read");
//     return err;

// }

FirebaseApp::FirebaseApp(const char* api_key)
    : https_certificate(cert_start), api_key(api_key)
{
    
    FirebaseApp::local_response_buffer = new char[HTTP_RECV_BUFFER_SIZE];
    FirebaseApp::register_url += FirebaseApp::api_key; 
    FirebaseApp::login_url += FirebaseApp::api_key;
    FirebaseApp::auth_url += FirebaseApp::api_key;
    firebaseClientInit();
}

FirebaseApp::~FirebaseApp()
{
    delete[] FirebaseApp::local_response_buffer;
    esp_http_client_cleanup(FirebaseApp::client);
}

esp_err_t FirebaseApp::registerUserAccount(const user_account_t& account)
{
    if (FirebaseApp::user_account.user_email != account.user_email || FirebaseApp::user_account.user_password != account.user_password)
    {
        FirebaseApp::user_account.user_email = account.user_email;
        FirebaseApp::user_account.user_password = account.user_password;
    }
    esp_err_t err = FirebaseApp::getRefreshToken(true);
    if (err != ESP_OK)
    {
        ESP_LOGE(FIREBASE_APP_TAG, "Failed to get refresh token");
        return ESP_FAIL;
    }
    FirebaseApp::clearHTTPBuffer();
    err = FirebaseApp::getAuthToken();
    if (err != ESP_OK)
    {
        ESP_LOGE(FIREBASE_APP_TAG, "Failed to get auth token");
        return ESP_FAIL;
    }
    FirebaseApp::clearHTTPBuffer();
    ESP_LOGI(FIREBASE_APP_TAG, "Created user successfully");

    return ESP_OK;
}

esp_err_t FirebaseApp::loginUserAccount(const user_account_t& account)
{
    if (FirebaseApp::user_account.user_email != account.user_email || FirebaseApp::user_account.user_password != account.user_password)
    {
        FirebaseApp::user_account.user_email = account.user_email;
        FirebaseApp::user_account.user_password = account.user_password;
    }
    esp_err_t err = FirebaseApp::getRefreshToken(false);
    if (err != ESP_OK)
    {
        ESP_LOGE(FIREBASE_APP_TAG, "Failed to get refresh token");
        return ESP_FAIL;
    }
    FirebaseApp::clearHTTPBuffer();

    err = FirebaseApp::getAuthToken();
    if (err != ESP_OK)
    {
        ESP_LOGE(FIREBASE_APP_TAG, "Failed to get auth token");
        return ESP_FAIL;
    }
    FirebaseApp::clearHTTPBuffer();
    ESP_LOGI(FIREBASE_APP_TAG, "Login to user successful");
    return ESP_OK;
}


}