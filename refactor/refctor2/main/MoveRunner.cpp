#include <Arduino.h>
#include "MoveRunner.h"

bool MoveRunner::isRunning[param.numAxes];
int MoveRunner::movedSteps[param.numAxes];
float MoveRunner::elapsedDur[param.numAxes];        //Elapsed time so far in accel/decel curve

MoveList* MoveRunner::moveList;
MovementPlanner* MoveRunner::plan;
Timers* MoveRunner::timers;

void MoveRunner::init(MoveList* list, MovementPlanner* movePlan, Timers* tim){
    reset();

    moveList = list;
    plan = movePlan;
    timers = tim;

    timers->init(X_AXIS_IT, Z_AXIS_IT, A_AXIS_IT);
}

void MoveRunner::reset(){
    for(int i = 0; i < param.numAxes; i++){
        isRunning[i] = false;
        movedSteps[i] = 0;
        elapsedDur[i] = 0;
    }
}

void MoveRunner::addNewMoves() {
    bool isStopped = true;
    Move* runMove;

    for(int i = 0; i < param.numAxes; i++){
        if(isRunning[i] == true)
            isStopped == false;
    }
    
    if(isStopped){
        runMove = moveList->getFirstRunnableMove();

        if(runMove->isRunning){
            moveList->releaseMove(runMove);
            reset();
            runMove = moveList->getFirstRunnableMove();
        }

        if(runMove->status == MOVE_STATUS_RUNNABLE){
            queueMove(runMove);
        }     
    }
}

void MoveRunner::queueMove(Move* runMove){
    plan->getFirstDelay(runMove);

    for(int i = 0; i < param.numAxes; i++){
        if(runMove->nextDelay[i] != 0){
            timers->setInterruptDelay(i, runMove->nextDelay[i]);
            timers->resumeInterrupt(i);
        }
    }

    runMove->isRunning = true;
}

void MoveRunner::X_AXIS_IT(HardwareTimer*){
    Move* runMove = moveList->getFirstRunnableMove();

    int axis = 0;

    if(runMove->dir[axis] == 1){
        digitalWrite(X_DIR,LOW);
    } else {
        digitalWrite(X_DIR,HIGH);
    }
    
    digitalWrite(X_STEP,LOW);
    delayMicroseconds(5);
    digitalWrite(X_STEP,HIGH);

    movedSteps[axis]++;
    
    if(movedSteps[axis] == runMove->numStartSteps[axis]){
        runMove->nextDelay[axis] = 1/runMove->steadySpeed[axis];
        elapsedDur[axis] = 0;
    }

    if(movedSteps[axis] > (runMove->numStartSteps[axis] + runMove->numSteadySteps[axis])){
        if(movedSteps[axis] < runMove->numSteps[axis]){
            elapsedDur[axis] += runMove->nextDelay[axis];
            plan->getNextDelay(runMove, false, axis, elapsedDur[axis]);
        } else {
            timers->stopInterrupt(axis);
            timers->resetCounter(axis);
            isRunning[axis] = true;
            addNewMoves();
            return;
        }
    } else if(movedSteps[axis] > runMove->numStartSteps[axis]){
        runMove->nextDelay[axis] = 1/runMove->steadySpeed[axis];
    } else {
        elapsedDur[axis] += runMove->nextDelay[axis];
        plan->getNextDelay(runMove, true, axis, elapsedDur[axis]);
    }


    timers->setInterruptDelay(axis, runMove->nextDelay[axis]);
}

void MoveRunner::Z_AXIS_IT(HardwareTimer*){
    Move* runMove = moveList->getFirstRunnableMove();

    int axis = 1;

    if(runMove->dir[axis] == 1){
        digitalWrite(Z_DIR,LOW);
    } else {
        digitalWrite(Z_DIR,HIGH);
    }
    
    digitalWrite(Z_STEP,LOW);
    delayMicroseconds(5);
    digitalWrite(Z_STEP,HIGH);

    movedSteps[axis]++;
    
    if(movedSteps[axis] == runMove->numStartSteps[axis]){
        runMove->nextDelay[axis] = 1/runMove->steadySpeed[axis];
        elapsedDur[axis] = 0;
    }

    if(movedSteps[axis] > (runMove->numStartSteps[axis] + runMove->numSteadySteps[axis])){
        if(movedSteps[axis] < runMove->numSteps[axis]){
            elapsedDur[axis] += runMove->nextDelay[axis];
            plan->getNextDelay(runMove, false, axis, elapsedDur[axis]);
        } else {
            timers->stopInterrupt(axis);
            timers->resetCounter(axis);
            isRunning[axis] = true;
            addNewMoves();
            return;
        }
    } else if(movedSteps[axis] > runMove->numStartSteps[axis]){
        runMove->nextDelay[axis] = 1/runMove->steadySpeed[axis];
    } else {
        elapsedDur[axis] += runMove->nextDelay[axis];
        plan->getNextDelay(runMove, true, axis, elapsedDur[axis]);
    }


    timers->setInterruptDelay(axis, runMove->nextDelay[axis]);
}

void MoveRunner::A_AXIS_IT(HardwareTimer*){
    Move* runMove = moveList->getFirstRunnableMove();

    int axis = 2;

    if(runMove->dir[axis] == 1){
        digitalWrite(A_DIR,LOW);
    } else {
        digitalWrite(A_DIR,HIGH);
    }
    
    digitalWrite(A_STEP,LOW);
    delayMicroseconds(5);
    digitalWrite(A_STEP,HIGH);

    movedSteps[axis]++;
    
    if(movedSteps[axis] == runMove->numStartSteps[axis]){
        runMove->nextDelay[axis] = 1/runMove->steadySpeed[axis];
        elapsedDur[axis] = 0;
    }

    if(movedSteps[axis] > (runMove->numStartSteps[axis] + runMove->numSteadySteps[axis])){
        if(movedSteps[axis] < runMove->numSteps[axis]){
            elapsedDur[axis] += runMove->nextDelay[axis];
            plan->getNextDelay(runMove, false, axis, elapsedDur[axis]);
        } else {
            timers->stopInterrupt(axis);
            timers->resetCounter(axis);
            isRunning[axis] = true;
            addNewMoves();
            return;
        }
    } else if(movedSteps[axis] > runMove->numStartSteps[axis]){
        runMove->nextDelay[axis] = 1/runMove->steadySpeed[axis];
    } else {
        elapsedDur[axis] += runMove->nextDelay[axis];
        plan->getNextDelay(runMove, true, axis, elapsedDur[axis]);
    }


    timers->setInterruptDelay(axis, runMove->nextDelay[axis]);
}

