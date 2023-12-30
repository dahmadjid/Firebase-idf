#ifndef _ESP_FIREBASE_FIRESTORE_H_
#define _ESP_FIREBASE_FIRESTORE_H_

#include "app.h"
#include "json/value.h"

namespace ESPFirebase {
struct HTTPResponse {
    esp_err_t err;
    int status_code;
    Json::Value body;
};

class Firestore {
public:
    Firestore(FirebaseApp& app, const char* project_id);
    HTTPResponse create_database(const char* database_id);
    HTTPResponse delete_database(const char* database_id);
    HTTPResponse get_database(const char* database_id);
    HTTPResponse list_databases();

    // HTTPResponse patch_database(const char* database_id);

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
        const char* database_id, const char* collection_id,
        const Document& document, const char* document_id = ""
    );

    HTTPResponse delete_document(
        const char* database_id, const char* collection_id,
        const char* document_id
    );

    DocumentResponse get_document(
        const char* database_id, const char* collection_id,
        const char* document_id
    );

    HTTPResponse patch_document(
        const char* database_id, const char* collection_id,
        std::vector<std::string> update_mask, const Document& document,
        const char* document_id = ""
    );

private:
    FirebaseApp& app;
    std::string project_id;
};

};  // namespace ESPFirebase
#endif