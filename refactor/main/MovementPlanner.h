#ifndef MOVEMENT_PLANNER_H
#define MOVEMENT_PLANNER_H

#include "Move.h"
#include "Param.h"
#include "CNCProcessor.h"

enum errorStatus {
  ok,
  outOfBounds,
  tooFast
};

class MovementPlanner {

  private:
    int pos[param.numAxes];
    int initializeCmd(Move &move); //Overall method which calls the following:
    void getCmdSteps(Move &move);
    void getCmdSpeed(Move &move);
    bool checkCmdParams(Move &move);
    void getMaxJunctSpeed(Move &move);
    
    void reserveMoves(Move &move);  //Reserves 'parseTime' milliseconds of movements as uneditable

    void findJunctSpeeds(Move &move);  //Further reduces intermediate speeds based on maximum acceleration limits
    
    void genMoveProfiles(Move &move);   //Generates lead-in and lead-out acceleration delays
    void genAccelDelays(float startSpeed[param.numAxes], float endSpeed[param.numAxes], float accelDelays[param.numAxes][param.maxAccelSteps], int numSteps[param.numAxes]);
  public:
    errorStatus plan(Move &move);
    void resetPos();
};
#endif