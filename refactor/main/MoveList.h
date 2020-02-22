#ifndef MOVE_LIST_H
#define MOVE_LIST_H

#include "Move.h"
#include "Param.h"

class MoveList {
  private:
    Move moveBuf[param.numMoveBuf];
    Move* startMove;
    Move* endMove;

  public:
    void init();
    bool hasFreeMove();
    Move* reserveMove();
    void releaseMove(Move* move);
    Move* getFirstRunnableMove();

};
#endif