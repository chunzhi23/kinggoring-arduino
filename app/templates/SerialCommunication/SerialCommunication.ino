#include <SoftwareSerial.h>

#define HC06_RXD 7
#define HC06_TXD 8

SoftwareSerial BTSerial(HC06_TXD, HC06_RXD);

void setup() {
  Serial.begin(9600);
  BTSerial.begin(9600);
}

void loop() {
  if (BTSerial.available()) {
    Serial.write(BTSerial.read());
  }
  if (Serial.available()) {
    BTSerial.write(Serial.read());
  }
}
