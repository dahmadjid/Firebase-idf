#include <iostream>
#include "esp_log.h"

#include "rtdb.h"

#include "jsoncpp/value.h"
#include "jsoncpp/json.h"
#define RTDB_TAG "RTDB"


namespace ESPFirebase {


RTDB::RTDB(FirebaseApp* app, const char * database_url)
    : app(app), base_database_url(database_url)

{
    
}
Json::Value RTDB::getData(const char* path)
{
    
    std::string url = RTDB::base_database_url;
    url += path;
    url += ".json?auth=" + this->app->auth_token;

    this->app->setHeader("content-type", "application/json");
    http_ret_t http_ret = this->app->performRequest(url.c_str(), HTTP_METHOD_GET, "");
    if (http_ret.err == ESP_OK && http_ret.status_code == 200)
    {
        const char* begin = this->app->local_response_buffer;
        const char* end = begin + strlen(this->app->local_response_buffer);

        Json::Reader reader;
        Json::Value data;

        reader.parse(begin, end, data, false);

        ESP_LOGI(RTDB_TAG, "Data with path=%s acquired", path);
        this->app->clearHTTPBuffer();
        return data;
    }
    else
    {   
        ESP_LOGE(RTDB_TAG, "Error while getting data at path %s| esp_err_t=%d | status_code=%d", path, (int)http_ret.err, http_ret.status_code);
        ESP_LOGI(RTDB_TAG, "Token expired ? Trying refreshing auth");
        esp_err_t err = this->app->loginUserAccount(this->app->user_account);
        this->app->setHeader("content-type", "application/json");
        http_ret = this->app->performRequest(url.c_str(), HTTP_METHOD_GET, "");
        if (http_ret.err == ESP_OK && http_ret.status_code == 200)
        {
            const char* begin = this->app->local_response_buffer;
            const char* end = begin + strlen(this->app->local_response_buffer);

            Json::Reader reader;
            Json::Value data;

            reader.parse(begin, end, data, false);

            ESP_LOGI(RTDB_TAG, "Data with path=%s acquired", path);
            this->app->clearHTTPBuffer();
            return data;
        }
        else
        {
            ESP_LOGE(RTDB_TAG, "Failed to get data after refreshing token. double check account credentials or database rules");
            this->app->clearHTTPBuffer();
            return Json::Value();
        }
    }
}

esp_err_t RTDB::putData(const char* path, const char* json_str)
{
    
    std::string url = RTDB::base_database_url;
    url += path;
    url += ".json?auth=" + this->app->auth_token;
    this->app->setHeader("content-type", "application/json");
    http_ret_t http_ret = this->app->performRequest(url.c_str(), HTTP_METHOD_PUT, json_str);
    this->app->clearHTTPBuffer();
    if (http_ret.err == ESP_OK && http_ret.status_code == 200)
    {
        ESP_LOGI(RTDB_TAG, "PUT successful");
        return ESP_OK;
    }
    else
    {
        ESP_LOGE(RTDB_TAG, "PUT failed");
        return ESP_FAIL;
    }
}

esp_err_t RTDB::putData(const char* path, const Json::Value& data)
{
    Json::FastWriter writer;
    std::string json_str = writer.write(data);
    esp_err_t err = RTDB::putData(path, json_str.c_str());
    return err;

}

esp_err_t RTDB::postData(const char* path, const char* json_str)
{
    
    std::string url = RTDB::base_database_url;
    url += path;
    url += ".json?auth=" + this->app->auth_token;
    this->app->setHeader("content-type", "application/json");
    http_ret_t http_ret = this->app->performRequest(url.c_str(), HTTP_METHOD_POST, json_str);
    this->app->clearHTTPBuffer();
    if (http_ret.err == ESP_OK && http_ret.status_code == 200)
    {
        ESP_LOGI(RTDB_TAG, "POST successful");
        return ESP_OK;
    }
    else
    {
        ESP_LOGE(RTDB_TAG, "POST failed");
        return ESP_FAIL;
        
    }
}

esp_err_t RTDB::postData(const char* path, const Json::Value& data)
{
    Json::FastWriter writer;
    std::string json_str = writer.write(data);
    esp_err_t err = RTDB::postData(path, json_str.c_str());
    return err;

}
esp_err_t RTDB::patchData(const char* path, const char* json_str)
{
    
    std::string url = RTDB::base_database_url;
    url += path;
    url += ".json?auth=" + this->app->auth_token;
    this->app->setHeader("content-type", "application/json");
    http_ret_t http_ret = this->app->performRequest(url.c_str(), HTTP_METHOD_PATCH, json_str);
    this->app->clearHTTPBuffer();
    if (http_ret.err == ESP_OK && http_ret.status_code == 200)
    {
        ESP_LOGI(RTDB_TAG, "PATCH successful");
        return ESP_OK;
    }
    else
    {
        ESP_LOGE(RTDB_TAG, "PATCH failed");
        return ESP_FAIL;
        
    }
}

esp_err_t RTDB::patchData(const char* path, const Json::Value& data)
{
    Json::FastWriter writer;
    std::string json_str = writer.write(data);
    esp_err_t err = RTDB::patchData(path, json_str.c_str());
    return err;
}


esp_err_t RTDB::deleteData(const char* path)
{
    
    std::string url = RTDB::base_database_url;
    url += path;
    url += ".json?auth=" + this->app->auth_token;
    this->app->setHeader("content-type", "application/json");
    http_ret_t http_ret = this->app->performRequest(url.c_str(), HTTP_METHOD_DELETE, "");
    this->app->clearHTTPBuffer();
    if (http_ret.err == ESP_OK && http_ret.status_code == 200)
    {
        ESP_LOGI(RTDB_TAG, "DELETE successful");
        return ESP_OK;
    }
    else
    {
        ESP_LOGE(RTDB_TAG, "DELETE failed");
        return ESP_FAIL;
    }
}


}