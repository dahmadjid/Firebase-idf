#ifndef _ESP_FIREBASE_H_
#define  _ESP_FIREBASE_H_
#include "esp_http_client.h"


#define HTTP_RECV_BUFFER_SIZE 4096

namespace ESPFirebase 
{

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
    /**
     * @brief Class over the esp_http_client, handles auth and should be passed as ptr to other classes such as RTDB 
     * 
     */
    class FirebaseApp 
        {
        private:
            const char* https_certificate;
            std::string api_key = "";
            std::string register_url = "https://identitytoolkit.googleapis.com/v1/accounts:signUp?key=";
            std::string login_url = "https://identitytoolkit.googleapis.com/v1/accounts:signInWithPassword?key=";
            std::string auth_url = "https://securetoken.googleapis.com/v1/token?key=";
            std::string refresh_token = "";
            esp_http_client_handle_t client;
            bool client_initialized = false;



            void firebaseClientInit(void);
        
            esp_err_t getRefreshToken(bool register_account);
            esp_err_t getAuthToken();
            esp_err_t nvsSaveTokens(); // useless until expire time added
            esp_err_t nvsReadTokens(); // useless until expire time added
            

        public:
            user_account_t user_account = {"", ""};

            char* local_response_buffer;

            std::string auth_token = "";

            /**
             * @brief Standard http request. Use after firebaseClientInit(). response stored in local_response_buffer. 
             * 
             * @param url Request url
             * @param method Request method
             * @param post_field Optional post field. Used when method is POST
             * @return Returns struct http_ret_t: esp_err_t + http status code.
             */
            http_ret_t performRequest(const char* url, esp_http_client_method_t method, std::string post_field = "");
            esp_err_t setHeader(const char* header, const char* value);
            
            void clearHTTPBuffer(void);
            
          

            FirebaseApp(const char * api_key);
            ~FirebaseApp();
            esp_err_t registerUserAccount(const user_account_t& account);
            esp_err_t loginUserAccount(const user_account_t& account);
        };
}




#endif