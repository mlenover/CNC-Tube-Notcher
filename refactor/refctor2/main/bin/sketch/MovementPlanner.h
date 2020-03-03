#ifndef MOVEMENT_PLANNER_H
#define MOVEMENT_PLANNER_H

#include "Move.h"
#include "Param.h"

enum errorStatus {
  ok,
  outOfBounds,
  tooFast
};

class MovementPlanner {
    public:
        void calcAccelDurations(Move* move);
        void calcAccelSteps(Move* move);
        void calcJerk(Move* move);
        void getFirstDelay(Move* move);
        void getNextDelay(Move* move, bool isAccel, int axis, float elapsedTime);
        void getParamsToRun(Move* move);
};

#endif