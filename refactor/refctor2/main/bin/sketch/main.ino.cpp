#include <Arduino.h>
#line 1 "c:\\Users\\MLeno\\Documents\\GitHub\\CNC-Tube-Notcher\\refctor2\\main\\main.ino"
#include "MoveList.h"
#include "Move.h"
#include "Param.h"
#include "MovementPlanner.h"
#include "Timers.h"
#include "MoveRunner.h"

#line 8 "c:\\Users\\MLeno\\Documents\\GitHub\\CNC-Tube-Notcher\\refctor2\\main\\main.ino"
void setDummyParams(Move* aMove);
#line 20 "c:\\Users\\MLeno\\Documents\\GitHub\\CNC-Tube-Notcher\\refctor2\\main\\main.ino"
void setup();
#line 38 "c:\\Users\\MLeno\\Documents\\GitHub\\CNC-Tube-Notcher\\refctor2\\main\\main.ino"
void loop();
#line 8 "c:\\Users\\MLeno\\Documents\\GitHub\\CNC-Tube-Notcher\\refctor2\\main\\main.ino"
void setDummyParams(Move* aMove){
  float del[3] = {10, 10, 0.2};
  float moveSpeed[3] = {10, 10, 0.2};

  for(int i = 0; i < param.numAxes; i++){
    aMove->entrySpeed[i] = 0;
    aMove->dir[i] = 1;
    aMove->steadySpeed[i] = moveSpeed[i]*param.stepsPer[i];
    aMove->numSteps[i] = floor(del[i]/param.stepsPer[i]);
  }
}

void setup() {
  MoveList moveList;
  static MovementPlanner plan;
  static MoveRunner run;
  static Timers timers;

  Serial.print("hi");
  moveList.init();
  Move* move = moveList.reserveMove();
  setDummyParams(move);
  plan.getParamsToRun(move);
  run.init(&moveList, &plan, &timers);

  run.addNewMoves();

  timers.resumeInterrupts();
}

void loop() {
}

