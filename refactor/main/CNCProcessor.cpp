#include "CNCProcessor.h"

void CNCProcessor::init(ICommandSource commandSource, IMoveRunner moveRunner)
{
  cs = commandSource;
  runner = moveRunner;
}

void CNCProcessor::run()
{
  stopProcessor = false;
  while (!stopProcessor)
  {
    if (moveList.hasFreeMove())
    {
      String data = cs.getData();
      if (data.length() != 0)
      {
        Move* mvt = moveList.reserveMove();
        if (mvt->cmd.parse(data))
        {
          
          switch(movementPlanner.plan(*mvt)) {
            case (outOfBounds):
              cs.printError("Error: commanded location out of range");
            case (tooFast):
              cs.printError("Error: commanded speed too fast");
            default:
              runner.add(moveList); 
          }
        }
        else {
          mvt->release();
        }
        if (moveList.hasFreeMove()) {
            cs.readyForData();
        }
        else {
          delay(100);
        }
      }
    }
  }
}

void CNCProcessor::end()
{
  stopProcessor = true;
}

