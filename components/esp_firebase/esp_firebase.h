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
    
    class Firebase 
    {
    private:
        const char* https_certificate;
        std::string api_key = "";
        std::string base_database_url;
        std::string register_url = "https://www.googleapis.com/identitytoolkit/v3/relyingparty/signupNewUser?key=";
        std::string login_url = "https://www.googleapis.com/identitytoolkit/v3/relyingparty/verifyPassword?key=";
        std::string auth_url = "https://securetoken.googleapis.com/v1/token?key=";
        std::string refresh_token = "";
        std::string auth_token = "";
        char* local_response_buffer;
        esp_http_client_handle_t client;
        bool client_initialized = false;
        void firebaseClientInit(void);
        
        esp_err_t getRefreshToken(const user_account_t& account, bool register_account);
        esp_err_t getAuthToken();
        
        std::string pathToURL(const char* path);

    public:
        Firebase(const config_t& config);
        ~Firebase();
        int registerUserAccount(const user_account_t& account);
        int loginUserAccount(const user_account_t& account);
        Json::Value getData(const char* path);
        
    };
}














#endif