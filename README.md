# Firebase in ESP-IDF


Firebase in esp-idf

## Features:
realtime database for now. other features later

## Getting Started:
- Make sure to have created a firebase project 
- Make sure to have created a realtime database 
- Clone this repository
- Open firebase_config.h and configure all of the macros correctly 
### Configure Wifi SSID and Password
- Add SSID and PASSWORD of your wifi network
- wifiInit function found in wifi_utils.h will connect to wifi
### Configure Auth :
#### If you want a quick setup:
- edit rules of database to {
  "rules": {
    ".read": true,
    ".write": true
    }
 }
#### If you want users and authentication:
- Create an api key (create credentials) and copy it in the API_KEY macro using [google cloud console](https://console.cloud.google.com/apis/credentials)
- Go to your firebase project authentication tab and add new provider Email/Password and press save.
- Create new user of your firebase project
- edit rules of database to {
  "rules": {
    ".read": "auth.uid != null",
    ".write": "auth.uid != null"
    }
 }
- Copy email and password of the newly created user to USER_EMAIL and USER_PASSWORD macros
- copy the link of the realtime database (not the website url) to DATABASE_URL 
