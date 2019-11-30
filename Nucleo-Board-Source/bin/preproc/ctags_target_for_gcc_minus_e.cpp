# 1 "c:\\Users\\MLeno\\Documents\\GitHub\\CNC-Tube-Notcher\\Nucleo-Board-Source\\main.h"
# 1 "c:\\Users\\MLeno\\Documents\\GitHub\\CNC-Tube-Notcher\\Nucleo-Board-Source\\main.ino"
# 2 "c:\\Users\\MLeno\\Documents\\GitHub\\CNC-Tube-Notcher\\Nucleo-Board-Source\\main.ino" 2

int i;
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(13, 0x1);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(13, 0x1); // turn the LED on (HIGH is the voltage level)
  i = 1;
  delay(1000); // wait for a second
  digitalWrite(13, 0x0); // turn the LED off by making the voltage LOW
  i = 0;
  delay(1000); // wait for a second
}
