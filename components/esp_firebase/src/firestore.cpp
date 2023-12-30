#include "esp_firebase/firestore.h"

namespace ESPFirebase {
using Document = Firestore::Document;
using DocumentResponse = Firestore::DocumentResponse;

Firestore::Firestore(FirebaseApp& app, const char* project_id)
    : app(app), project_id(project_id) {}

HTTPResponse Firestore::create_database(const char* database_id) {
    std::cout << "NOT IMPLEMENTED\n";
    return {};
}

HTTPResponse Firestore::delete_database(const char* database_id) {
    std::cout << "NOT IMPLEMENTED\n";
    return {};
}

HTTPResponse Firestore::get_database(const char* database_id) {
    std::cout << "NOT IMPLEMENTED\n";
    return {};
}

HTTPResponse Firestore::list_databases() {
    std::cout << "NOT IMPLEMENTED\n";
    return {};
}

Json::Value Document::dump_json() {
    std::cout << "NOT IMPLEMENTED\n";
    return {};
}

DocumentResponse create_document(
    const char* database_id, const char* collection_id,
    const Document& document, const char* document_id = ""
) {
    std::cout << "NOT IMPLEMENTED\n";
    return {};
}

HTTPResponse delete_document(
    const char* database_id, const char* collection_id, const char* document_id
) {
    std::cout << "NOT IMPLEMENTED\n";
    return {};
}

DocumentResponse get_document(
    const char* database_id, const char* collection_id, const char* document_id
) {
    std::cout << "NOT IMPLEMENTED\n";
    return {};
}

HTTPResponse patch_document(
    const char* database_id, const char* collection_id,
    std::vector<std::string> update_mask, const Document& document,
    const char* document_id = ""
) {
    std::cout << "NOT IMPLEMENTED\n";
    return {};
}
};  // namespace ESPFirebase