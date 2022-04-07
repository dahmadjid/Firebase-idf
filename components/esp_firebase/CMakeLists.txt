
idf_component_register(SRCS "esp_firebase.cpp"
                    INCLUDE_DIRS "." ".."
                    PRIV_REQUIRES esp_http_client esp-tls
                    EMBED_TXTFILES gtsr1.pem)
