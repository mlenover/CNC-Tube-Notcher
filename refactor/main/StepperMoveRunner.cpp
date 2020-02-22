#include "StepperMoveRunner.h"


void StepperMoveRunner::init(MoveList &list) {
    moveList = &list;
}


void StepperMoveRunner::add(MoveList moveList) {
    bool isStopped = true;
    Move* runMove;

    for(int i = 0; i < param.numAxes; i++){
        if(isRunning[i] == true)
            isStopped == false;
    }

    runMove = moveList.getFirstRunnableMove();
    
    if(isStopped){
        if(startMove->status == MOVE_STATUS_RUNNABLE){
            queueMove(startMove);
        }     
    }
}


void queueMove(Move* move){
    float delay;
    //Find where to start move
    for(int i = 0; i < param.numAxes; i++){
        if(move->numStartSteps[i] != 0){
            delay = move->startAccel[i][0];
        } else if(move->numSteadySteps != 0){
            delay = 1/move->steadySpeed[i];
        } else {
            continue;
        }
        setInterrupt(i, delay);
    }
}

Move* StepperMoveRunner::getNextMoveToRun(){
    if(startMove == nullptr) {
        return nullptr;
    } else
    {
        Move* move = startMove;
        startMove = startMove->nextMove;
        startMove->prevMove = nullptr;
        return move;
    }
    
}

void StepperMoveRunner::doneRunningMove(Move &move){

}
