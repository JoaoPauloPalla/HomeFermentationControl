// #include <Arduino.h>
// #include <Wifi.h>
// #include <ESPAsyncWebServer.h>
// #include <LittleFS.h>
// #include <max6675.h>

// unsigned long lastTime = 0;  
// unsigned long timerDelay = 10000;
// const char* ssid = "Jean";
// const char* password = "2020brito";
// const int ledPin = 2;
// String ledState;
// AsyncWebServer server(80);
// AsyncEventSource events("/events");

#include <dcm.h>

DataControllerModule* dcm;
void setup() {
  Serial.begin(9600);
  pinMode(2, OUTPUT);
  pinMode(27, OUTPUT);  
  digitalWrite(27, HIGH);
  dcm = new DataControllerModule();
  dcm->run();
}

void loop() {
  delay(2000);
}


// int thermoDO = 4;
// int thermoCS = 5;
// int thermoCLK = 6;
// MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);


// String processor(const String& var){
//   Serial.println(var);
//   if(var == "STATE"){
//     if(digitalRead(ledPin)){
//       ledState = "ON";
//     }
//     else{
//       ledState = "OFF";
//     }
//     Serial.print(ledState);
//     return ledState;
//   }
//   return String(" ");
// }

// void setup() {
  // Serial.begin(9600);
  // pinMode(ledPin, OUTPUT);

  // Serial.print("Mounting LittleFS...");
  // if(!LittleFS.begin(true)){
  //   Serial.println("An error has occurred while mounting LittleFS");
  //   return;
  // }
  // if(LittleFS.exists("/index.html") && LittleFS.exists("/style.css") && LittleFS.exists("/script.js")){
  //   Serial.println("LittleFS mounted successfully");
  // }

  // WiFi.begin(ssid, password);
  // while(WiFi.status() != WL_CONNECTED){
  //   delay(1000);
  //   Serial.println("Connecting to WiFi..");
  // }
  // Serial.println(WiFi.localIP());

  // server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
  //   request->send(LittleFS, "/index.html", "text/html", false, processor);
  // });
  // server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
  //   request->send(LittleFS, "/style.css", "text/css");
  // });
  // server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request){
  //   request->send(LittleFS, "/script.js", "text/javascript");
  // });

  // server.on("/state", HTTP_GET, [](AsyncWebServerRequest *request){
  //   if (request->hasParam("state")){
  //     AsyncWebParameter* p = request->getParam("state");
  //     if(p->value() == "ON"){
  //       digitalWrite(ledPin, HIGH);
  //       ledState = "ON";
  //     }
  //     else if(p->value() == "OFF"){
  //       digitalWrite(ledPin, LOW);
  //       ledState = "OFF";
  //     }
  //     events.send(String(ledState).c_str(),"state",millis(),1000);
  //     request->send(200, "text/plain", "OK");
  //   }
  //   else{
  //     request->send(400, "text/plain", "Error, no state!");
  //   }
  // });

  // events.onConnect([](AsyncEventSourceClient *client){
  //   client->send("hello!",NULL,millis(),1000);
  // });

  // server.addHandler(&events);
  // server.begin();
// }


// void loop() {
  // if ((millis() - lastTime) > timerDelay) {
    
  //   events.send(String(lastTime).c_str(), "temperature", millis());
  //   lastTime = millis();
  // }
// }
