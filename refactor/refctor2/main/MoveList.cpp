#include "MoveList.h"
#include <Arduino.h> /*****/

Move* MoveList::startMove;
Move* MoveList::endMove;


void MoveList::init(){
    // NOTE: param.numMoveBuf must not be less than 3.
    Move* firstMove = new Move;
    Move* lastMove = new Move;
    firstMove = moveBuf[0];
    lastMove = moveBuf[param.numMoveBuf-1];
    firstMove->nextMove = moveBuf[1];
    firstMove->prevMove = lastMove;

    for(int i = 1; i < param.numMoveBuf-1; i++){  // Skip first and last items in array, so 
        Move* move = new Move;
        move = moveBuf[i];
        move->prevMove = moveBuf[i-1];
        move->nextMove = moveBuf[i+1];
        moveBuf[i]->status = MOVE_STATUS_UNUSED;
    }
    firstMove->status = MOVE_STATUS_UNUSED;
    lastMove->status = MOVE_STATUS_UNUSED;
    Move* startMove = firstMove;
    Move* endMove = firstMove;

    lastMove->prevMove = moveBuf[param.numMoveBuf-2];
    lastMove->nextMove = firstMove;
}

bool MoveList::hasFreeMove(){
    return ((endMove->status == MOVE_STATUS_UNUSED) || (endMove->nextMove->status == MOVE_STATUS_UNUSED));
}

Move* MoveList::reserveMove(){   
    if(endMove->status == MOVE_STATUS_UNUSED){
        endMove->status = MOVE_STATUS_EDITABLE;
        return endMove;
    }

    if(endMove->nextMove->status == MOVE_STATUS_UNUSED){
        endMove->nextMove->status = MOVE_STATUS_EDITABLE;
        endMove = endMove->nextMove;
        return endMove;
    }

    return nullptr;
}

void MoveList::releaseMove(Move* move){
    if(move == nullptr){
        return;
    }

    if(move == startMove && move != endMove){
        startMove = startMove->nextMove;
    }

    move->status = MOVE_STATUS_UNUSED;
    return;
}

Move* MoveList::getFirstRunnableMove(){
    if(startMove->status == MOVE_STATUS_RUNNABLE){
        return startMove;
    }

    return nullptr;
}
