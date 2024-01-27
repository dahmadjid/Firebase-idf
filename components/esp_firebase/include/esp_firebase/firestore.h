#ifndef _ESP_FIREBASE_FIRESTORE_H_
#define _ESP_FIREBASE_FIRESTORE_H_

#include "app.h"
#include "json/value.h"
#include <unordered_map>

namespace ESPFirebase {
struct HTTPResponse {
    esp_err_t err;
    int status_code;
    Json::Value body;
};

class Firestore {
public:
    Firestore(FirebaseApp& app, const std::string& project_id);
    HTTPResponse create_database(const std::string& database_id);
    HTTPResponse delete_database(const std::string& database_id);
    HTTPResponse get_database(const std::string& database_id);
    HTTPResponse list_databases();
    // HTTPResponse patch_database(const std::string& database_id);

/*
"arrayValue": {
"values": [
    {
    "booleanValue": false
    }
]
},
"booleanValue": false,
"bytesValue": "",
"doubleValue": 0,
"geoPointValue": {},
"integerValue": 0,
"mapValue": {
"fields": {
    "": {
    "arrayValue": {}
    }
}
},
"nullValue": "NULL_VALUE",
"referenceValue": "",
"stringValue": "",
"timestampValue": ""
*/

    struct Document {
        std::string name;
        Json::Value fields;
        std::string create_time;  // In response only
        std::string update_time;  // In response only
        Json::Value dump_json();
    };

    struct DocumentResponse {
        HTTPResponse response;
        Document document;
    };

    DocumentResponse create_document(
        const std::string& database_id, const std::string& collection_id,
        const Document& document, const std::string& document_id = ""
    );

    HTTPResponse delete_document(
        const std::string& database_id, const std::string& collection_id,
        const std::string& document_id
    );

    DocumentResponse get_document(
        const std::string& database_id, const std::string& collection_id,
        const std::string& document_id
    );

    HTTPResponse patch_document(
        const std::string& database_id, const std::string& collection_id,
        std::vector<std::string> update_mask, const Document& document,
        const std::string& document_id = ""
    );

private:
    std::string make_url(const std::string& path, bool path_contains_query);
    FirebaseApp& app;
    std::string base_url;
};

};  // namespace ESPFirebase
#endif