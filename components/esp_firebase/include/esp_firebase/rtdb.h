#ifndef _ESP_FIREBASE_RTDB_H_
#define _ESP_FIREBASE_RTDB_H_

#include "app.h"
#include "json/json.h"
#include "json/value.h"

namespace ESPFirebase {
class RTDB {
   public:
    Json::Value getData(const char* path);

    esp_err_t putData(const char* path, const char* json_str);
    esp_err_t putData(const char* path, const Json::Value& data);

    esp_err_t postData(const char* path, const char* json_str);
    esp_err_t postData(const char* path, const Json::Value& data);

    esp_err_t patchData(const char* path, const char* json_str);
    esp_err_t patchData(const char* path, const Json::Value& data);

    esp_err_t deleteData(const char* path);
    RTDB(FirebaseApp& app, const char* database_url);

   private:
    FirebaseApp& app;
    std::string base_database_url;
};
}  // namespace ESPFirebase

#endif