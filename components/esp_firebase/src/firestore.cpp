// #include "esp_firebase/firestore.h"

// namespace ESPFirebase {
// using Document = Firestore::Document;
// using DocumentResponse = Firestore::DocumentResponse;

// Firestore::Firestore(FirebaseApp& app, const std::string& base_url)
//     : app(app), base_url(base_url) {}

// std::string build_path(const std::string& database_id, const std::string& collection_id) {
//     std::string path; 
//     path.reserve(250);
//     sprintf(path.data(), "databases/%s/documents/%s", database_id, collection_id);
//     return path;
// }

// std::string Firestore::make_url(const std::string& path, bool path_contains_query) {
//     if (path_contains_query) {
//         return this->base_url + path + "&key=" + this->app.auth_token;
//     } else {
//         return this->base_url + path + "?key=" + this->app.auth_token;
//     }
// }

// HTTPResponse Firestore::create_database(const std::string& database_id) {
//     std::cout << "NOT IMPLEMENTED\n";
//     return {};
// }

// HTTPResponse Firestore::delete_database(const std::string& database_id) {
//     std::cout << "NOT IMPLEMENTED\n";
//     return {};
// }

// HTTPResponse Firestore::get_database(const std::string& database_id) {
//     std::cout << "NOT IMPLEMENTED\n";
//     return {};
// }

// HTTPResponse Firestore::list_databases() {
//     std::cout << "NOT IMPLEMENTED\n";
//     return {};
// }

// Json::Value Document::dump_json() {
//     Json::Value out;
//     out["name"] = this->name;
//     out["fields"] = this->fields; 
//     out["createTime"] = this->create_time; 
//     out["updateTime"] = this->update_time; 
//     return out;
// }

// DocumentResponse Firestore::create_document(
//     const std::string& database_id, const std::string& collection_id,
//     const Document& document, const std::string& document_id = ""
// ) {
//     std::string path = build_path(database_id, collection_id);

//     // const std::string url = this->make_url(path, );
//     return {};
// }

// HTTPResponse Firestore::delete_document(
//     const std::string& database_id, const std::string& collection_id, const std::string& document_id
// ) {
//     std::cout << "NOT IMPLEMENTED\n";
//     return {};
// }

// DocumentResponse Firestore::get_document(
//     const std::string& database_id, const std::string& collection_id, const std::string& document_id
// ) {
//     std::cout << "NOT IMPLEMENTED\n";
//     return {};
// }

// HTTPResponse Firestore::patch_document(
//     const std::string& database_id, const std::string& collection_id,
//     std::vector<std::string> update_mask, const Document& document,
//     const std::string& document_id = ""
// ) {
//     std::cout << "NOT IMPLEMENTED\n";
//     return {};
// }
// };  // namespace ESPFirebase