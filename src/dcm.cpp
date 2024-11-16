#include <dcm.h>

DataControllerModule::DataControllerModule() {
    
    scanTime = 1;
    temperature = 0;
    maxTemperature = 0.0;
    minTemperature = 50.0;

    // thermocouple = SCLK, CS, SO
    thermocouple = new MAX6675(13, 12, 14);

    Serial.print("Mounting LittleFS...");
    if(!LittleFS.begin(true)){
        Serial.println("An error has occurred while mounting LittleFS");
        return;
    }
    if(LittleFS.exists("/index.html") && LittleFS.exists("/style.css") && LittleFS.exists("/script.js") ){
        Serial.println("LittleFS mounted successfully");
    }

    ip = IPAddress(192, 168, 1, 10);
    gateway =  IPAddress(192, 168, 1, 254);
    subnet =  IPAddress(255, 255, 255, 0);
    ssid = "JOÃO";
    password = "1234567@a";

    // Configures static IP address
    if (!WiFi.config(ip, gateway, subnet)) {
        Serial.println("STA Failed to configure");
    }
    WiFi.begin(ssid, password);
    while(WiFi.status() != WL_CONNECTED){
        delay(1000);
        Serial.println("Connecting to WiFi..");
    }
    Serial.println(WiFi.localIP());

    Serial.println("Initializing webserver.");
    Events = new AsyncEventSource("/events");    
    Webserver = new AsyncWebServer(80);
    
    configureEndpoints();

    Events->onConnect([](AsyncEventSourceClient *client){
        client->send("hello!",NULL,millis(),1000);
    });
    
}

DataControllerModule::~DataControllerModule() {
    delete Events;
    delete Webserver;
}


void DataControllerModule::run() {
    //Initialize webserver
    Webserver->addHandler(Events);
    Webserver->begin();
    Serial.println("Webserver started.");

    //Initialize task for temperature control
    xTaskCreatePinnedToCore(DataControllerModule::taskTemperatureControl, "taskTemperatureControl", 8192, (void *)this, 1, NULL, 1);
}

void DataControllerModule::taskTemperatureControl(void *args) {
    DataControllerModule *dcm = (DataControllerModule *)args;
    float temperature;
    while(1){
        temperature = dcm->thermocouple->readCelsius();    

        //Update temperature in webserver
        dcm->Events->send(String(temperature).c_str(),"temperatureEvent");


        if (temperature > dcm->maxTemperature) {
            Serial.println("Max temperature: " + String(temperature));
            dcm->maxTemperature = temperature;
            dcm->Events->send(String(temperature).c_str(),"maxTemperatureEvent");
        }

        if (temperature < dcm->minTemperature) {
            Serial.println("Min temperature: " + String(temperature));
            dcm->minTemperature = temperature;
            dcm->Events->send(String(temperature).c_str(),"minTemperatureEvent");
        }

        //Control temperature
        dcm->controlTemperature(temperature);

        vTaskDelay(dcm->scanTime*60000);
    }
}

void DataControllerModule::controlTemperature(float temperature) {
    float currentTemperature = temperature;

    //Control temperature with Hysteresis
    if (currentTemperature >= temperatureSetpoint + temperatureHysteresis) {
        gpio_set_level(GPIO_NUM_27, 1); // Define o pino 27 como alto
    } else if (currentTemperature <= temperatureSetpoint - temperatureHysteresis) {
        gpio_set_level(GPIO_NUM_27, 0); // Define o pino 27 como baixo
    }
}

void DataControllerModule::configureEndpoints() {
    /*GET*/
    Webserver->on("/", HTTP_GET, [this](AsyncWebServerRequest *request){
        
        String data;
        StaticJsonDocument<200> jsonDoc;
        jsonDoc["temperature"] = this->temperature;
        jsonDoc["maxTemperature"] = this->maxTemperature;
        jsonDoc["minTemperature"] = this->minTemperature;
        jsonDoc["temperatureSetpoint"] = this->temperatureSetpoint;
        jsonDoc["temperatureHysteresis"] = this->temperatureHysteresis;
        jsonDoc["scanTime"] = this->scanTime;
        serializeJson(jsonDoc, data);
        Events->send(data.c_str(),"onload");

        request->send(LittleFS, "/index.html", "text/html");
    });
    Webserver->on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(LittleFS, "/style.css", "text/css");
    });
    Webserver->on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(LittleFS, "/script.js", "text/javascript");
    }); 

    /*POST*/
        Webserver->on("/state", HTTP_POST,
        [](AsyncWebServerRequest *request ) { },
        nullptr,
        [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {            
            JsonDocument jsonDoc;
            deserializeJson(jsonDoc, data, len);            
            String state = jsonDoc["state"];
            if(state == "ON"){
                digitalWrite(2, HIGH);
                digitalWrite(15, LOW);
                digitalWrite(16, LOW);
                request->send(200, "text/plain", "LED ON");
            } else if (state == "OFF") {
                digitalWrite(2, LOW);
                digitalWrite(15, HIGH);
                digitalWrite(16, HIGH);
                request->send(200, "text/plain", "LED OFF");
            }else{                
                request->send(400, "text/plain", "BAD REQUEST");
                return;
            }
        }
    ); 


    Webserver->on("/scanTime", HTTP_POST,
        [](AsyncWebServerRequest *request ) { },
        nullptr,
        [this](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {            
            JsonDocument jsonDoc;
            deserializeJson(jsonDoc, data, len);            
            float newScanTime = jsonDoc["scanTime"].as<float>();        
            if (newScanTime > 0 ){
                this->scanTime = newScanTime;
            }

            Serial.println("Scan time updated: " + String(this->scanTime));
            Events->send(String(this->scanTime).c_str(),"scanTimeEvent");
            request->send(200, "text/plain", "OK");        
        }
    );

    Webserver->on("/setPoint", HTTP_POST,
        [](AsyncWebServerRequest *request ) { },
        nullptr,
        [this](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {            
            JsonDocument jsonDoc;
            deserializeJson(jsonDoc, data, len);            
            float newSetpoint = jsonDoc["setPoint"].as<float>();        
            this->temperatureSetpoint = newSetpoint;
            Serial.println("Setpoint updated: " + String(this->temperatureSetpoint));
            Events->send(String(this->temperatureSetpoint).c_str(),"setPointEvent");
            request->send(200, "text/plain", "OK");        
        }
    );

    Webserver->on("/hysteresis", HTTP_POST,
        [](AsyncWebServerRequest *request ) { },
        nullptr,
        [this](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {            
            JsonDocument jsonDoc;
            deserializeJson(jsonDoc, data, len);            
            float newHysteresis = jsonDoc["scanTime"].as<float>();        
            this->temperatureHysteresis = newHysteresis;
            Serial.println("Hysteresis updated: " + String(this->temperatureHysteresis));
            Events->send(String(this->temperatureHysteresis).c_str(),"hysteresisEvent");
            request->send(200, "text/plain", "OK");        
        }
    );
}