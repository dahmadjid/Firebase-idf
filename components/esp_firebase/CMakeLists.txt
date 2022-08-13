
idf_component_register(SRCS "rtdb.cpp" "app.cpp"
                    INCLUDE_DIRS "." ".."
                    PRIV_REQUIRES esp_http_client esp-tls
                    EMBED_TXTFILES gtsr1.pem)
