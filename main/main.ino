#ifndef GCodeCommand_h
#include "GCodeCommand.h"
#endif

#ifndef Movement_h
#include "Movement.h"
#endif

#ifndef Main_h
#include "main.h"
#endif

bool isSteady[NUM_AXIES] = {true, true, true};
int remainingSteps[NUM_AXIES] = {0, 0, 0};
float stepDelay[NUM_AXIES][MAX_MOVE_STEPS];
int numCommands = 0;
bool isMoving = false;

char serialBuffer[MAX_BUF];
String command;
int sofar;

bool absMode = true;
float pos[NUM_AXIES] = {0, 0, 0};

GCodeCommand *CodeBuf[2];

HardwareTimer *xAxisTim = new HardwareTimer(TIM1);
HardwareTimer *zAxisTim = new HardwareTimer(TIM2);
HardwareTimer *aAxisTim = new HardwareTimer(TIM3);

HardwareTimer *Tim[NUM_AXIES] = {xAxisTim, zAxisTim, aAxisTim};

void xAxisInterrupt(HardwareTimer*)
{
  stepMotor(0);
  remainingSteps[0] = remainingSteps[0] - 1;
  //Serial.println(remainingSteps[0]);
  if(isSteady[0] && remainingSteps[0] > 0){
    Tim[0]->setOverflow(stepDelay[0][0], MICROSEC_FORMAT);
  } else {
    Tim[0]->pause();
  }
}


void zAxisInterrupt(HardwareTimer*)
{
  stepMotor(1);
  remainingSteps[1] = remainingSteps[1] - 1;

  if(isSteady[1] && remainingSteps[1] > 0){
    Tim[1]->setOverflow(stepDelay[1][0], MICROSEC_FORMAT);
  } else {
    Tim[1]->pause();
  }
}


void aAxisInterrupt(HardwareTimer*)
{
  stepMotor(2);
  remainingSteps[2] = remainingSteps[2] - 1;

  if(isSteady[2] && remainingSteps[2] > 0){
    Tim[2]->setOverflow(stepDelay[2][0], MICROSEC_FORMAT);
  } else {
    Tim[2]->pause();
  }
}

void readLimSwitches(bool (&switchState) [NUM_AXIES]){
  for(int i=0; i < NUM_AXIES; i++){
    switchState[i] = digitalRead(pin.limPin[i]);
  }
}

void stepMotor(int axis){
  digitalWrite(pin.stepPin[axis], HIGH);
  digitalWrite(pin.stepPin[axis], LOW);
}

void setDir(bool dir, int axis){
  if(dir){
    digitalWrite(pin.dirPin[axis],HIGH);
  } else {
    digitalWrite(pin.dirPin[axis],LOW);
  }
}
  
void pinSetup(){
  pinMode(ENABLE_PIN, OUTPUT);
  for (int i = 0; i < NUM_AXIES; i++){
    pinMode(pin.stepPin[i], OUTPUT);
    pinMode(pin.dirPin[i], OUTPUT);
  }
}

void charToString(char S[], String &D)
{
 String rc(S);
 D = rc;
}

void setInterrupt(int axis){
    Tim[axis]->setMode(1, TIMER_OUTPUT_COMPARE);
  Tim[axis]->setOverflow(stepDelay[axis][0], MICROSEC_FORMAT);
  switch(axis) {
        case 0:
            Tim[axis]->attachInterrupt(xAxisInterrupt);
            break;
        case 1:
            Tim[axis]->attachInterrupt(zAxisInterrupt);
            break;
        case 2:
            Tim[axis]->attachInterrupt(aAxisInterrupt);
            break;
  }
  Tim[axis]->resume();
}

void processCommand() {
  charToString(serialBuffer, command);
  GCodeCommand *g = new GCodeCommand();

  bool ok = g->parse(command);
  if( ok == true) {
      gcode code = g->getCode();
      bool hasParam[3];
      double x = g->getParameter('X', hasParam[0]);
      double z = g->getParameter('Z', hasParam[1]);
      double a = g->getParameter('A', hasParam[2]);

      /*
      Serial.print("Code: ");
      Serial.println(code);
      if(hasParam[0]){
        Serial.print("X Value: ");
        Serial.println(x);
      }
      if(hasParam[1]){
      Serial.print("Z Value: ");
      Serial.println(z);
      }
      if(hasParam[2]){
      Serial.print("A Value: ");
      Serial.println(a);
      }
      */

      switch (code) {
        case G00:
          // statements
          break;
        case G01:
          // statements
          break;
        case G28:
          break;
        case G90:
          break;
        case G91:
          break;
        case M02:
          break;
        default:
          // statements
          break;
     }

      if(code == G00 || code == G01 || code == M02){
        if(numCommands == 0){
          CodeBuf[0] = g;
        } else {
          CodeBuf[1] = g;
        }
        numCommands++;
      } else if(code == G90) {
        absMode = true;
      } else if(code == G91) {
        absMode = false;
      }
  }
}

void ready() {
  sofar = 0;
  Serial.print(F(">"));  
}

void setup(){
    Serial.begin(115200);
    pinSetup();
    digitalWrite(ENABLE_PIN, LOW);
    ready();
}

void loop(){
  while(Serial.available()){
    char c=Serial.read();
    Serial.print(c);
    if(sofar<MAX_BUF-1) serialBuffer[sofar++]=c;
    if(c=='\n') {
      serialBuffer[sofar]=0;
      Serial.print(F("\r\n"));
      processCommand();
      if(numCommands < 2){
        ready();
      }
    }
  }

  if(numCommands > 0){
    if(!isMoving){
      executeCommand(CodeBuf, pos, absMode, isSteady);
    } else {
      if(isFinishedMove()){
        isMoving = false;
        CodeBuf[0] = CodeBuf[1];
        numCommands--;
        ready();
      }
    }
  }
}
