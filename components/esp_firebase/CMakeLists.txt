
idf_component_register(
    SRCS "src/rtdb.cpp" "src/app.cpp" "src/firestore.cpp"
    INCLUDE_DIRS "include"
    REQUIRES jsoncpp esp_http_client
    PRIV_REQUIRES esp-tls
    EMBED_TXTFILES gtsr1.pem
)