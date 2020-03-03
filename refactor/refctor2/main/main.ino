#include "MoveList.h"
#include "Move.h"
#include "Param.h"
#include "MovementPlanner.h"
#include "Timers.h"
#include "MoveRunner.h"

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
  Serial.begin(115200);
  
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
