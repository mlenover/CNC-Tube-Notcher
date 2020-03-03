#ifndef MOVE_H
#define MOVE_H

#include "Param.h"

#define MOVE_STATUS_UNUSED 0
#define MOVE_STATUS_EDITABLE 1
#define MOVE_STATUS_RUNNABLE 2

class Move {
    public:
        float entrySpeed[param.numAxes] = {0,0,0};      //Steady speed to begin movement at
        float steadySpeed[param.numAxes];               //Commanded speed for each axis
        //float maxEntrySpeed[param.numAxes] = {0,0,0};   //Max value of entryScale based on centripital acceleration thru movement change
        //float accel[param.numAxes] = {0,0,0};           //Acceleration of each axis
        float leadInJerk[param.numAxes] = {0,0,0};                           //Rate of change of acceleration during lead-in accel
        float leadOutJerk[param.numAxes] = {0,0,0};                          //Rate of change of acceleration during lead-out accel
        float leadInDur = 0;                            //Time to spend accelerating during lead-in
        float leadOutDur = 0;                           //Time to spend accelerating during lead-out
        float nextDelay[param.numAxes] = {0,0,0};       //Time to wait before next step;
        int numSteps[param.numAxes];                    //Total number of steps for movement
        int numStartSteps[param.numAxes];               //Number of steps in lead-in acceleration
        int numSteadySteps[param.numAxes];              //Number of steps steady-state movement
        int numEndSteps[param.numAxes];                 //Number of steps in lead-out acceleration
        //int startStep[param.numAxes];                   //Step position at beginning of movement
        int dir[param.numAxes];                         //Direction of each axis movement
        //bool isOptimal = false;                         //Should this movement be checked again when another move is added?
        //bool isNewlyOptimal = false;                    //Flag set during junction speed calculations, since one more delays must be calculated once more
        bool isRunning = false;                         //Has started being executed
        Move* prevMove = nullptr;                       //Pointer to previous movement
        Move* nextMove = nullptr;                       //Pointer to next movement
        byte status = MOVE_STATUS_UNUSED;               //Status of move; 0:Unused, 1:Editable, 2:Runable
        //GCodeCommand cmd;

        void release();


};

#endif