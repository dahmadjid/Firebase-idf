#ifndef _ESP_FIREBASE_H_
#define  _ESP_FIREBASE_H_

#include <iostream>
#include "esp_http_client.h"
//AIzaSyDrjXuyDkljaJ9IDDIP8M4wbyBs2IUPB88
#define RECV_BUFFER_SIZE 4096
#define HTTP_TAG "HTTP_CLIENT"

namespace ESPFirebase 
{

    struct config_t
    {
        char* api_key; 
        char* database_url;
    };

    struct user_account_t
    {
        char* user_email;
        char* user_password;
    };
    
    class Firebase 
    {
    private:
        char* https_certificate;
        std::string api_key = "";
        std::string refresh_token = "";
        std::string auth_token = "";
        std::string register_url = "https://www.googleapis.com/identitytoolkit/v3/relyingparty/signupNewUser?key=";
        std::string login_url = "https://www.googleapis.com/identitytoolkit/v3/relyingparty/verifyPassword?key=";
        std::string auth_url = "https://securetoken.googleapis.com/v1/token?key=";
        std::string base_database_url;
        
        char local_response_buffer[RECV_BUFFER_SIZE];
        esp_http_client_handle_t client;



        esp_err_t http_event_handler(esp_http_client_event_t *evt);
        esp_err_t firebaseClientInit(void);
        int getRefreshToken(const user_account_t& account);
        int getAuthToken();
        std::string getData(const char* path);

    public:
        Firebase(const config_t& config);
        
        int registerUserAccount(const user_account_t& account);
        int loginUserAccount(const user_account_t& account);
        
    };
}














#endif