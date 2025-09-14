// RENAME THIS FILE TO config.h

#ifndef CONFIG_H
#define CONFIG_H

#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define SSID1 "Totalplay-2.4G-db70"
#define PSWD1 "6y6uYnQLCsEFF8ma"
#define SSID2 "carlos"
#define PSWD2 "cacacaca"
#define SSID3 "Dime_lindura0"
#define PSWD3 "lindura0"
#define SSID4 "hola"
#define PSWD4 "holahola"

#define WIFI_TIME_OUT 500
#define MAX_WIFI_CONNECT_TIME 15000

#define PROJECT_ENDPOINT "https://esli-game-api.onrender.com/api/highscores" // change the port if necessary
#define CONCURSO_ENDPOINT "https://pabloalcaraz.mx/PicoChallenge/SpringEdition_2025/data/scores/hook.php"
#define API_KEY "esligod"
#define DEVICE_NAME "ESLI Laboratorios"

#endif