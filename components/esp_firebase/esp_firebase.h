#ifndef _ESP_FIREBASE_H_
#define  _ESP_FIREBASE_H_

#include <iostream>
#include "esp_http_client.h"

#include "jsoncpp/value.h"
#include "jsoncpp/json.h"

#define HTTP_RECV_BUFFER_SIZE 4096

namespace ESPFirebase 
{

    struct config_t
    {
        const char* api_key; 
        const char* database_url;
    };

    struct user_account_t
    {
        const char* user_email;
        const char* user_password;
    };
    
    struct http_ret_t
    {
        esp_err_t err;
        int status_code;
    }; 

    class Firebase 
    {
    private:
        const char* https_certificate;
        std::string api_key = "";
        std::string base_database_url;
        std::string register_url = "https://identitytoolkit.googleapis.com/v1/accounts:signUp?key=";
        std::string login_url = "https://identitytoolkit.googleapis.com/v1/accounts:signInWithPassword?key=";
        std::string auth_url = "https://securetoken.googleapis.com/v1/token?key=";
        std::string refresh_token = "";
        std::string auth_token = "";
        char* local_response_buffer;
        esp_http_client_handle_t client;
        bool client_initialized = false;
        account_t user_account = {"", ""};



        void firebaseClientInit(void);
        esp_err_t setHeader(const char* header, const char* value);
        http_ret_t performClient(const char* url, esp_http_client_method_t method, std::string post_field);

        esp_err_t getRefreshToken(const user_account_t& account, bool register_account);
        esp_err_t getAuthToken();

        esp_err_t saveTokensToNVS();
        std::string pathToURL(const char* path);

    public:
        Firebase(const config_t& config);
        ~Firebase();
        esp_err_t registerUserAccount(const user_account_t& account);
        esp_err_t loginUserAccount(const user_account_t& account);
        Json::Value getData(const char* path);
        esp_err_t putData(const char* path, const Json::Value& data);
        
    };
}














#endif