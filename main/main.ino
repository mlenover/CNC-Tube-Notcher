#ifndef GCodeCommand_h
#include "GCodeCommand.h"
#endif

#ifndef Movement_h
#include "Movement.h"
#endif

#ifndef Main_h
#include "main.h"
#endif

#include <RingBuf.h>

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

float zeros[NUM_AXIES] = {0, 0, 0};
    
RingBuf<float*, bufSize> interSpeed;
RingBuf<float*, bufSize> cmdSpeed;
RingBuf<GCodeCommand*, bufSize> commandBuf;

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

void commandQueue(){
  
}

void addCmdToBuf() {
  charToString(serialBuffer, command);
  GCodeCommand *g = new GCodeCommand();

  bool ok = g->parse(command);
  if( ok == true) {
      gcode code = g->getCode();
      
      switch (code) {
        case G90: //Absolute Coordinates
          absMode = true;
          break;
        case G91: //Relative Coordinates
          absMode = false;
          break;
        case UNKNOWN:
          break;
        default:
          commandBuf.push(g); //Add command to buffer
      }
  }
}

void processCmd(){
  int numCmds = commandBuf.size();
  float speeds[NUM_AXIES];
  bool hasSpeed;
  gcode code;
  if(numCmds > cmdSpeed.size()){
    for(int i = cmdSpeed.size(); i < numCmds; i++){
      code = commandBuf[i]->getCode();
      if(code == G00){
        getCmdSpeed(commandBuf[i], speeds, pos, absMode);
        cmdSpeed.push(speeds);
      } else if(code == G01){
        eccf
      } else {
        cmdSpeed.push(zeros);
      }
    }
  }
  if(numCmds > interSpeed.size()){
      if(code == G00){
      } else if(code == G01){
        
      } else {
        cmdSpeed.push(zeros);
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
    interSpeed.push(zeros);
    
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
      addCmdToBuf();
      processCmd();
      if(numCommands < bufSize){
        ready();
      }
    }
  }

  if(numCommands > 0){
    if(!isMoving){
      //executeCommand(CodeBuf, pos, absMode, isSteady);
    } else {
      if(isFinishedMove()){
        isMoving = false;
        for(int i = 0; i < (bufSize - 1); i++){
          CodeBuf[i] = CodeBuf[i+1];
          numCommands--;
        }
        ready();
      }
    }
  }
}
