#ifndef MOVE_LIST_H
#define MOVE_LIST_H

#include "Move.h"
#include "Param.h"

class MoveList {
  private:
    Move* moveBuf[param.numMoveBuf];
    static Move* startMove;
    static Move* endMove;

  public:
    MoveList* create();
    void init();
    bool hasFreeMove();
    Move* reserveMove();
    void releaseMove(Move* move);
    static Move* getFirstRunnableMove();

};

#endif