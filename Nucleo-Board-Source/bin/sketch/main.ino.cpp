#include <Arduino.h>
#line 1 "c:\\Users\\MLeno\\Documents\\GitHub\\CNC-Tube-Notcher\\Nucleo-Board-Source\\main.ino"
/*
  DUMMY BLINK CODE
*/
int i;
// the setup function runs once when you press reset or power the board
#line 6 "c:\\Users\\MLeno\\Documents\\GitHub\\CNC-Tube-Notcher\\Nucleo-Board-Source\\main.ino"
void setup();
#line 12 "c:\\Users\\MLeno\\Documents\\GitHub\\CNC-Tube-Notcher\\Nucleo-Board-Source\\main.ino"
void loop();
#line 6 "c:\\Users\\MLeno\\Documents\\GitHub\\CNC-Tube-Notcher\\Nucleo-Board-Source\\main.ino"
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  i = 1;
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  i = 0;
  delay(1000);                       // wait for a second
}

