#include "SerialCommandSource.h"
#include <Arduino.h>

void SerialCommandSource::readyForData() {
  Serial.print('>');
}

String SerialCommandSource::getData() {
  String data = "";
  while (Serial.available()) {
    c = Serial.read();
    Serial.print(c);
    if (c == '\n') {
      break;
    }
    else {
      data += c;
    }
  }
  return data;
}


void SerialCommandSource::printError(String error) {
  Serial.print(error);
}