#ifndef MOVE_RUNNER_H
#define MOVE_RUNNER_H

#include "Param.h"
#include "Move.h"
#include "MoveList.h"
#include "MovementPlanner.h"
#include "Timers.h"

class MoveList;
class MovementPlanner;
class Timers;

class MoveRunner {

    private:
        Move *startMove;
        Move *endMove;

        static bool isRunning[param.numAxes];
        static int movedSteps[param.numAxes];
        static float elapsedDur[param.numAxes];        //Elapsed time so far in accel/decel curve

        static MoveList* moveList;
        static MovementPlanner* plan;
        static Timers* timers;

        static void reset();

    public:
        static void init(MoveList* list, MovementPlanner* movePlan, Timers* tim);
        static void addNewMoves();
        static void checkIfDone();
        static void queueMove(Move* runMove);

        static void X_AXIS_IT(HardwareTimer*);
        static void Z_AXIS_IT(HardwareTimer*);
        static void A_AXIS_IT(HardwareTimer*);

};

#endif