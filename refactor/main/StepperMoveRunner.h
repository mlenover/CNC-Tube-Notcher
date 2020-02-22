#ifndef STEPPER_MOVE_RUNNER_H
#define STEPPER_MOVE_RUNNER_H

#include "MovementPlanner.h"
#include "MoveList.h"
#include "Move.h"
#include "IMoveRunner.h"
#include "Param.h"
#include "main.h"
#include <Arduino.h>

class StepperMoveRunner:IMoveRunner {

  private:
    Move *startMove;
    Move *endMove;
    MoveList* moveList;
    void queueMove(Move* move);

  public:
    bool isRunning[param.numAxes] = {false, false, false};
    virtual void init(MoveList &list);
    virtual void add(MoveList moveList);
    virtual Move* getNextMoveToRun();
    virtual void doneRunningMove(Move &move);
};
#endif