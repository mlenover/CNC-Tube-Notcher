#ifndef CNC_PROCESSOR_H
#define CNC_PROCESSOR_H

#include "Param.h"
#include "ICommandSource.h"
#include "SerialCommandSource.h"
#include "MoveList.h"
#include "MovementPlanner.h"
#include "IMoveRunner.h"

class CNCProcessor {

  private:
    ICommandSource cs;
    GCodeCommand command;
    bool stopProcessor;
    IMoveRunner runner;
    MovementPlanner movementPlanner;
    MoveList moveList;

  public:
    CNCProcessor(ICommandSource commandSource);
    void init(ICommandSource commandSource, IMoveRunner moveRunner);
    void run();
    void pause();
    void resume();
    void end();
};

#endif
