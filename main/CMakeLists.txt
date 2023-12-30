
idf_component_register(
    SRCS "main.cpp"
    REQUIRES esp_firebase esp_wifi nvs_flash
    INCLUDE_DIRS "." 
)
