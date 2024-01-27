#include <iostream>

#include "esp_firebase/rtdb.h"
#include "esp_firebase/firestore.h"
#include "esp_log.h"
#include "firebase_config.h"
#include "json/value.h"
#include "wifi_utils.h"
using namespace ESPFirebase;
extern "C" void app_main(void) {
    wifiInit(SSID, PASSWORD);  // blocking until it connects
    // Config and Authentication
    // user_account_t account = {USER_EMAIL, USER_PASSWORD};
    // FirebaseApp app = FirebaseApp(API_KEY);
    // app.loginUserAccount(account);

    // std::cout << app.auth_token << "\n";
    // std::cout << API_KEY << "\n";
}
