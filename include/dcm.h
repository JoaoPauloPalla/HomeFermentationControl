#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <AsyncJson.h>
#include <ESPAsyncWebServer.h>
#include <max6675.h>
#include <LittleFS.h>
#include <WiFi.h>


class DataControllerModule{
    public:
        DataControllerModule();
        ~DataControllerModule();

        IPAddress ip;
        IPAddress gateway;
        IPAddress subnet;
        const char* ssid;
        const char* password;

        int scanTime;        
        float temperature;
        float maxTemperature;
        float minTemperature;
        
        float temperatureSetpoint;
        float temperatureHysteresis;        

        AsyncWebServer* Webserver;
        AsyncEventSource* Events;
        MAX6675* thermocouple;
        void run();
        static void taskTemperatureControl(void* args); 
        void controlTemperature( float mesuredTemperature );
        void configureEndpoints();

};