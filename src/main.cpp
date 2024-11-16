#include <dcm.h>

DataControllerModule* dcm;
void setup() {
  Serial.begin(9600);
  pinMode(2, OUTPUT);
  pinMode(15, OUTPUT);  
  pinMode(16, OUTPUT);
  digitalWrite(15, HIGH);
  digitalWrite(16, HIGH);
  dcm = new DataControllerModule();
  dcm->run();
}

void loop() {
  delay(2000);
}
