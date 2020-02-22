#include "Move.h"

void Move::release() {
    if(prevMove != nullptr) {
        prevMove->nextMove = nullptr;
    }
    prevMove = nullptr;
    if(nextMove != nullptr) {
        nextMove->prevMove = nullptr;
    }
    nextMove = nullptr;
    status = MOVE_STATUS_UNUSED;
}




