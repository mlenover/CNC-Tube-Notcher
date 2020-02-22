#ifndef IMOVE_RUNNER_H
#define IMOVE_RUNNER_H

#include "Move.h"

class IMoveRunner {
  public:
    void init(MoveList &list);
    void add(MoveList moveList);
    Move* getNextMoveToRun();
    void doneRunningMove(Move &move);
};
#endif