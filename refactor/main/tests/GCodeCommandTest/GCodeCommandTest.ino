#include "..\..\GCodeCommand.cpp"
#include <AUnit.h>
using aunit::TestRunner;

GCodeCommand gc;

void setup(){
  Serial.begin(115200);
  TestRunner::setPrinter(&Serial);
}

void loop(){
  TestRunner::run();  
}

test(parseTest){
  gc.setup();
  boolean parseOK = gc.parse("G01 X1 Y2 Z3");
  assertTrue(parseOK);
  
  // Verify code
  gcode code = gc.getCode();
  assertEqual(code, G01);
  bool hasP;
  
  // Verify X parameter
  double p = gc.getParameter('X', hasP);
  assertTrue(hasP);
  assertEqual(p, 1.0);
  
  // Verify Y
  p = gc.getParameter('Y', hasP);
  assertTrue(hasP);
  assertEqual(p, 2.0);
  
  // Verify correct response to missing parameter
  gc.getParameter('Q', hasP);
  assertFalse(hasP);
}
