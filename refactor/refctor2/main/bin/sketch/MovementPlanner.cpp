#include "MovementPlanner.h"

void MovementPlanner::calcAccelDurations(Move* move){
    float duration = 0;
    float maxDur = 0;

    for(int i = 0 ; i < param.numAxes; i++){
        if(move->steadySpeed[i] != 0){
            duration = param.precision * (move->steadySpeed[i]-move->entrySpeed[i]) / param.maxAccel[i];
            if(duration > maxDur){
                maxDur = duration;
            }
        }
    }

    move->leadInDur = duration;

    duration = 0;

    if(move->nextMove != MOVE_STATUS_UNUSED){
        for(int i = 0 ; i < param.numAxes; i++){
            if(move->steadySpeed[i] != 0){
                duration = param.precision * (move->steadySpeed[i]-move->nextMove->entrySpeed[i]) / param.maxAccel[i];
                if(duration > maxDur){
                    maxDur = duration;
                }
            }
        }
    }   else {
        for(int i = 0 ; i < param.numAxes; i++){
            if(move->steadySpeed[i] != 0){
                duration = param.precision * move->steadySpeed[i] / param.maxAccel[i];
                if(duration > maxDur){
                    maxDur = duration;
                }
            }
        }
    }

    move->leadOutDur = duration;
}

void MovementPlanner::calcJerk(Move* move){
    float accel;

    for(int i = 0; i < param.numAxes; i++){
        accel = (move->steadySpeed[i]-move->entrySpeed[i])/(move->leadInDur/param.precision);
        move->leadInJerk[i] = 2*accel/(move->leadInDur/param.precision);
    }

    if(move->nextMove->status != MOVE_STATUS_UNUSED){
        for(int i = 0; i < param.numAxes; i++){
            accel = (move->steadySpeed[i]-move->nextMove->entrySpeed[i])/(move->leadOutDur/param.precision);
            move->leadOutJerk[i] = 2*accel/(move->leadOutDur/param.precision);
        }
    } else {
        for(int i = 0; i < param.numAxes; i++){
            accel = move->steadySpeed[i]/(move->leadOutDur/param.precision);
            move->leadOutJerk[i] = 2*accel/(move->leadOutDur/param.precision);
        }
    }
}

void MovementPlanner::calcAccelSteps(Move* move){
    for(int i = 0; i < param.numAxes; i++){
        move->numStartSteps[i] = move->leadInDur*(move->entrySpeed[i]+move->steadySpeed[i])/2;

        if(move->nextMove->status != MOVE_STATUS_UNUSED){
            for(int i = 0; i < param.numAxes; i++){
                move->numEndSteps[i] = move->leadInDur*(move->nextMove->entrySpeed[i]+move->steadySpeed[i])/2;
            }
        } else {
            for(int i = 0; i < param.numAxes; i++){
                move->numEndSteps[i] = move->leadInDur*move->steadySpeed[i]/2;
            }
        }

        move->numSteadySteps[i] = move->numSteps[i] - move->numStartSteps[i] - move->numEndSteps[i];
    }
}

void MovementPlanner::getFirstDelay(Move* move){
    float duration;
    float jerk;
    float nextSpeed;

    float c1;
    float c2;

    for(int i = 0; i < param.numAxes; i++){
        if(move->numSteadySteps[i] > 0 && move->numStartSteps[i] == 0){
            move->nextDelay[i] = 1/move->steadySpeed[i];
            continue;
        }

        if(move->numStartSteps[i] > 0){
            c1 = -1/(move->leadInJerk[i]/3);
            c2 = move->entrySpeed[i]/(move->leadInJerk[i]/2);
        } else {
            if(move->nextMove->status != MOVE_STATUS_UNUSED){
                c1 = 1/(move->leadInJerk[i]/3);
                c2 = -1*move->nextMove->entrySpeed[i]/(move->leadInJerk[i]/2);
            } else {
                c1 = 1/(move->leadInJerk[i]/3);
                c2 = 0;
            }
        }

        move->nextDelay[i] = param.precision*cbrt(-1*c1+sqrt(pow(c1,2)+pow(c2,3)))+cbrt(-1*c1-sqrt(pow(c1,2)+pow(c2,3)));
   }
}

void MovementPlanner::getParamsToRun(Move* move){
    calcAccelDurations(move);
    calcJerk(move);
    calcAccelSteps(move);
}

void MovementPlanner::getNextDelay(Move* move, bool isAccel, int axis, float elapsedTime){
    if(isAccel){
        if(elapsedTime < (move->leadInDur/2)){
            move->nextDelay[axis] = param.precision/(move->leadInJerk[axis]*sq(elapsedTime/param.precision));
        } else {
            move->nextDelay[axis] = param.precision/(-move->leadInJerk[axis]*sq((elapsedTime-move->leadInDur)/param.precision)+move->steadySpeed[axis]);
        }
    } else {
        if(elapsedTime < (move->leadOutDur/2)){
            move->nextDelay[axis] = param.precision/(-move->leadOutJerk[axis]*sq(elapsedTime/param.precision)+move->steadySpeed[axis]);
        } else {
            move->steadySpeed[axis] = param.precision/(move->leadOutJerk[axis]*sq((elapsedTime-move->leadOutDur)/param.precision));
        }
    }
}