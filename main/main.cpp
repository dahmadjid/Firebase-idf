
#include <iostream>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"


#include "jsoncpp/value.h"
#include "jsoncpp/json.h"

#include "esp_firebase/esp_firebase.h"
#include "wifi_utils.h"

// Wifi Credentials
#define SSID "SSID"
#define PASSWORD "PASSWORD"

// Read readme.md to properly configure api key and authentication

// create a new api key and add it here 
#define API_KEY "API_KEY"
// Copy your firebase real time database link here 
#define DATABASE_URL "https://project-name-default-rtdb.europe-west1.firebasedatabase.app/"  

#define USER_EMAIL "test@gmail.com"   // This gmail does not exist. it only exists in the firebase project as a user
#define USER_PASSWORD "testpass"      // Dont add your gmail credentials. Setup users authentication in your Firebase project first


extern "C" void app_main(void)
{
    wifiInit(SSID, PASSWORD);  // blocking until it connects

    ESPFirebase::config_t config = {API_KEY, DATABASE_URL};
    ESPFirebase::user_account_t account = {USER_EMAIL, USER_PASSWORD};

    ESPFirebase::Firebase fb_client(config);

    fb_client.loginUserAccount(account);

    Json::Value root = fb_client.getData("test");

    Json::Value::Members memebers = root.getMemberNames();

    std::string data = root["test"].asString();

    ESP_LOGI("MAIN", "test = %s", data.c_str());

}
