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
float stepDelay[NUM_AXIES][MAX_ACCEL_STEPS];
float maxAccel[NUM_AXIES] = {X_MAX_ACCEL, A_MAX_ACCEL, Z_MAX_ACCEL};
int numCommands = 0;
bool isMoving = false;

char serialBuffer[MAX_BUF];
String command;
int sofar;

bool absMode = true;
int pos[NUM_AXIES] = {0, 0, 0};

float zeros[NUM_AXIES] = {0, 0, 0};
float maxSpeeds[NUM_AXIES] = {X_MAX_SPEED, Z_MAX_SPEED, A_MAX_SPEED};

RingBuf<Move*, bufSize> moveBuf;
RingBuf<GCodeCommand*, bufSize> commandBuf;
RingBuf<float, bufSize> dummyBuffer;

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
  int numMoves = moveBuf.size();
  float oldEndSpeeds[NUM_AXIES];
  
  /*
  int numMoves = moveBuf.size();
  float speeds[NUM_AXIES];
  float* nextSpeeds;
  bool hasSpeed;
  gcode code;
  */

  int numSteps[NUM_AXIES];
  
  if(numCmds > 0){
    //Parse new command for first time

    //Create new movement object
    Move *m = new Move();

    //Save the current position
    m->startStep = pos;

    //From command, save number of steps
    getNumSteps(commandBuf[numCmds-1], absMode, m->startStep, m->numSteps);  

    //From command, save steady speed
    getCmdSpeed(commandBuf[numCmds-1], m->steadySpeed, pos, m->numSteps);

    //Based on turn sharpness, set max junction speed
    if(numMoves > 0){
      getMaxJunctScale(m->maxEntrySpeed, moveBuf[numMoves-1]->steadySpeed, m->steadySpeed, m->startStep);
    } else {
    //If this is the first movement to be processed, set the max entry speed to 0
      m->maxEntrySpeed = 0;
    }

    //Check if junction speed exceeds previous or current nominal speed. Reduce if necessary
    if(numMoves > 0){
      if(m->maxEntrySpeed > moveBuf[numMoves-1]->steadySpeed){
        m->maxEntrySpeed = moveBuf[numMoves-1]->steadySpeed;
      }

      if(m->maxEntrySpeed > m->steadySpeed){
        m->maxEntrySpeed = m->steadySpeed;
      }
    }

    //Set the current entry speed to the maximum entry speed
    m->entrySpeed = m->maxEntrySpeed;

    //Add the new movement to the buffer
    moveBuf.push(m);

    numMoves++;

    bool isOptimalNow[numMoves];
    float currentDuration = 0;
    float totalDuration = 0;
    int lAxis = 0;
    int mostSteps = 0;
    float topSpeed[NUM_AXIES = {0,0,0};
    float leadInDur = 0;
    float leadOutDur = 0;
    float maxSpeedScale = 0;

    
    for(int i = numMoves - 1; i > 1; i--){
      isOptimalNow[i] = false;
      if(i == (numMoves - 1)){
        getMaxAccelToSpeed(0, moveBuf[i]->entrySpeed, moveBuf[i]->numSteps);
      } else {
        getMaxAccelToSpeed(moveBuf[i+1]->entrySpeed, moveBuf[i]->entrySpeed, moveBuf[i]->numSteps);
      }
      
      if(compareSpeeds(moveBuf[i]->entrySpeed,moveBuf[i]->maxEntrySpeed)){
        moveBuf[i]->entrySpeed = moveBuf[i]->maxEntrySpeed;
        
        for(int j = i; j > 1; j--){
          isOptimalNow[j] = true;
        }
        
        i = 1;  //This means all subsequent moves are unchanged, as entry speeds are already set to max entry speeds
      }
    }

    for(int i = 1; i < (numMoves - 1); i++){
      oldEndSpeeds = moveBuf[i+1]->entrySpeed;
      getMaxAccelToSpeed(moveBuf[i]->entrySpeed, moveBuf[i+1]->entrySpeed, moveBuf[i]->numSteps);

      if(compareSpeeds(moveBuf[i+1]->entrySpeed,oldEndSpeeds)){
        moveBuf[i+1]->entrySpeed = oldEndSpeeds;
      } else if(isOptimalNow[i]){ //Else->exit speed had to be reduced, not optimal
        isOptimalNow[i] = false;
      }
    }
    
    for(int i = 1; i < numMoves; i++){
      if(!(moveBuf[i]->isOptimal)){  //Calculate movement profiles for everything previously not optimal

        //First, determine the axis that can accelerate the slowest
        for(int j=0; j< NUM_AXIES; j++){
          currentDuration = (nominalSpeed[i]-junctSpeed[i])/maxAccel[i];
          if(currentDuration > totalDuration){
              totalDuration = currentDuration;  
              lAxis = j;
          }
        }

        //For this axis, how many steps would it take to reach maximum speed, while still being able to reach exit junction speed?
        //NOT necessarily reach nominal speed; this may not be possible

        if(i < (numMoves - 1)){
          moveBuf[i]->numStartSteps[lAxis] = (sq(moveBuf[i]->entrySpeed[i]) - sq(moveBuf[i+1]->entrySpeed[i]) + 2*maxAccel[lAxis]*moveBuf[i]->numSteps[lAxis])/(4*maxAccel[lAxis]);
        } else {  //Last item; end junction speed set to 0
          moveBuf[i]->numStartSteps[lAxis] = (sq(moveBuf[i]->entrySpeed[i]) + 2*maxAccel[lAxis]*moveBuf[i]->numSteps[lAxis])/(4*maxAccel[lAxis]);
        }

        //Find the maximum speed for the slowest accelerating axis
        topSpeed[lAxis] = sqrt(sq(moveBuf[i]->entrySpeed[i]) + 2*maxAccel[lAxis]*moveBuf[i]->numStartSteps[lAxis]);

        //How long do we have for lead in/lead out?
        if(topSpeed[lAxis] > moveBuf[i]->steadySpeed[lAxis]){
          leadInDur = (topSpeed[lAxis] - moveBuf[i]->entrySpeed[i])/maxAccel[lAxis];
          if(i < (numMoves - 1)){
            leadOutDur = (moveBuf[i+1]->entrySpeed[i] - topSpeed[lAxis])/maxAccel[lAxis];
          } else {
            leadOutDur = (-1*topSpeed[lAxis])/maxAccel[lAxis];
          }
        } else {
          leadInDur = (moveBuf[i]->steadySpeed[lAxis] - moveBuf[i]->entrySpeed[i])/maxAccel[lAxis];
          if(i < (numMoves - 1)){
            leadOutDur = (moveBuf[i+1]->entrySpeed[i] - moveBuf[i]->steadySpeed[lAxis])/maxAccel[lAxis];
          } else {
            leadOutDur = (-1*moveBuf[i]->steadySpeed[lAxis])/maxAccel[lAxis];
          }
        }

        //What percent of our top speed can we reach?
        maxSpeedScale = topSpeed/moveBuf[i]->steadySpeed[lAxis];

        //For each axis,
        for(int j=0; j<NUM_AXIES; j++){

          //Calculate the top speed we can reach
          topSpeed[j] = moveBuf[i]->steadySpeed[j]*maxSpeedScale;

          //Calculate the number of lead-in steps
          moveBuf[i]->numStartSteps = 0.5*(maxSpeedScale*moveBuf[i]->steadySpeed[j] + moveBuf[i]->entrySpeed[j])*leadInDur;

          //Calculate the number of lead-out steps,
          if(i < (numMoves - 1)){
            moveBuf[i]->numEndSteps[j] = 0.5*(maxSpeedScale*moveBuf[i]->steadySpeed[j] + moveBuf[i+1]->entrySpeed[j])*leadInDur;
          } else {
            moveBuf[i]->numEndSteps[j] = 0.5*(maxSpeedScale*moveBuf[i]->steadySpeed[j])*leadInDur;  //If we are on the last move, end at 0
          }

          //As a final check, in case of off-by-one errors, reduce the number of steps in lead-in and lead-out until they do not exceed total num steps
          while((moveBuf[i]->numStartSteps[j] + moveBuf[i]->numEndSteps[j]) > moveBuf[i]->numSteps[j]){
            moveBuf[i]->numStartSteps[j]--;

            if((moveBuf[i]->numStartSteps[j] + moveBuf[i]->numEndSteps[j]) > moveBuf[i]->numSteps[j]){
              moveBuf[i]->numEndSteps[j]--;
            }
          }

          //Any remaining steps occur at a steady maximum speed
          moveBuf[i]->numSteadySteps[j] = moveBuf[i]->numSteps[j] - moveBuf[i]->numStartSteps[j] - moveBuf[i]->numEndSteps[j];

          //Generate the lead-in delays
          getAccelDelays(moveBuf[i]->entrySpeed, topSpeed[j], moveBuf[i]->numStartSteps, moveBuf[i]->dir, leadInDur, true, moveBuf[i]->startAccel);

          //Generate the lead-out delays
          if(i < (numMoves - 1)){
            getAccelDelays(moveBuf[i+1]->entrySpeed, topSpeed[j], moveBuf[i]->numEndSteps, moveBuf[i]->dir, leadOutDur, false, moveBuf[i]->startAccel);
          } else {
            getAccelDelays({0,0,0}, topSpeed[j], moveBuf[i]->numEndSteps, moveBuf[i]->dir, leadOutDur, false, moveBuf[i]->startAccel);
          }
        }
        
      }
      moveBuf[i]->isOptimal = isOptimalNow[i];
    }

    
  }
  /*
  if(numCmds >= numMoves){   //moveBuf[0] is always PREVIOUS movement, so need 2 commands in buffer before new command is parsed
    for(int i = numMoves; i <= numCmds; i++){
      Move *m = new Move();
      getNumSteps(commandBuf[i-1], absMode, pos, m->numSteps); //Calculate the number of steps for each axis, and save into new move object
      
      code = commandBuf[i-1]->getCode();
      if(code == G00){
        m->steadySpeed = maxSpeeds;
      } else if(code == G01){
        getCmdSpeed(commandBuf[i-1], speeds, pos, m->numSteps);
        m->steadySpeed = speeds;
      } else {
        m->steadySpeed = zeros;
      }

      //calc start accel
      
      moveBuf.push(m);
      
      if(moveBuf.size() > 2){
        getAccelToSpeed(moveBuf[i-1]->steadySpeed, moveBuf[i]->steadySpeed, moveBuf[i-1]->endSpeed); //Based on previous steady speed and current steady speed, set transition speed between previous & current moves
      }
      //calc end accel
      
      //calc num start, steady, end steps
    }
  }
  */
}

void ready() {
  sofar = 0;
  Serial.print(F(">"));  
}

void setup(){
    Serial.begin(115200);
    pinSetup();
    digitalWrite(ENABLE_PIN, LOW);  

    Move *m = new Move();
    //m->endSpeed = zeros;
    moveBuf.push(m);
    
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
