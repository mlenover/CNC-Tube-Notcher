#include "MovementPlanner.h"

errorStatus MovementPlanner::plan(Move &move){
    int errorCode;

    move.status = MOVE_STATUS_EDITABLE;
    errorCode = initializeCmd(move);

    if(errorCode == 0){
        findJunctSpeeds(move);
        genMoveProfiles(move);
        reserveMoves(move);
    }

    switch (errorCode){
        case 1:
            return outOfBounds;
        case 2:
            return tooFast;
        default:
            return ok;
    }
}

int MovementPlanner::initializeCmd(Move &move){
    int errorCode;

    getCmdSteps(move);
    getCmdSpeed(move);
    errorCode = checkCmdParams(move);

    if(errorCode == 0){
        getMaxJunctSpeed(move);
    }

    return errorCode;
}

void MovementPlanner::getCmdSteps(Move &move){
    float val;
    bool hasParam[param.numAxes];

    if(move.cmd.absMode){
        for(int i = 0; i < param.numAxes; i++){
            val = move.cmd.getParameter(param.axes[i],hasParam[i]);
            if(hasParam[i]){
                move.numSteps[i] = round(val*param.stepsPer[i]) - pos[i];
                pos[i] = round(val*param.stepsPer[i]);

                if(move.numSteps[i] < 0){
                    move.numSteps[i] = -1*move.numSteps[i];
                    move.dir[i] = -1;
                } else {
                    move.dir[i] = 1;
                }
            } else {
                move.numSteps[i] = 0;
                move.dir[i] = 1;
            }
        }
    } else {
        for(int i = 0; i < param.numAxes; i++){
            val = move.cmd.getParameter(param.axes[i],hasParam[i]);
            if(hasParam[i]){
                move.numSteps[i] = round(val*param.stepsPer[i]);
                pos[i] = move.numSteps[i] + val;

                if(val >= 0){
                    move.dir[i] = -1;
                    move.numSteps[i] = -1*move.numSteps[i];
                } else {
                    move.dir[i] = 1;
                }
            }  else {
                move.numSteps[i] = 0;
                move.dir[i] = 1;
            }
        }
    }
}

void MovementPlanner::getCmdSpeed(Move &move){
    bool hasFeedrate;
    float fr = move.cmd.getParameter('F',hasFeedrate);

    if(!hasFeedrate){
        fr = param.minFeedrate;
    }
        
    float r1;
    float r2;
    
    float x_speed;
    float z_speed;
    float a_speed;
    
    float delta_x = move.numSteps[0];
    float delta_z = move.numSteps[1];
    float delta_a = move.numSteps[2];
    
    float r_avg = (delta_z+2*pos[1])/2;

    if (delta_z != 0) {
        z_speed = fr / sqrt(sq(delta_x/delta_z) + sq(r_avg)*sq(delta_a/delta_z) + 1);
        a_speed = (delta_a / delta_z)*z_speed;
        x_speed = (delta_x / delta_z)*z_speed;
    }
    else if (delta_a != 0){
        z_speed = 0;
        a_speed = fr / sqrt(sq(r_avg) + sq(delta_x/delta_a));
        x_speed = (delta_x / delta_a) * a_speed;
    }
    else {
        z_speed = 0;
        a_speed = 0;
        x_speed = fr;
    }

    move.steadySpeed[0] = abs(x_speed);
    move.steadySpeed[1] = abs(z_speed);
    move.steadySpeed[2] = abs(a_speed);

    return; 
}

bool MovementPlanner::checkCmdParams(Move &move){
    for(int i = 0; i < param.numAxes; i++){
        if(pos[i] > param.maxBound[i] || pos[i] < param.minBound[i]){
            for(int j = 0; j < param.numAxes; j++){
                pos[j] = pos[j] - move.dir[j]*move.numSteps[j];
            }
            return false;
        }
    }

    for(int i = 0; i < param.numAxes; i++){
        if(move.steadySpeed[i] > param.maxSpeed[i]){
            return false;
        }
    }
    
    return true;
}

void MovementPlanner::getMaxJunctSpeed(Move &move){
    float previousCartSpeed[param.numAxes]; //Cartesian step speed
    //We will define x-y plane to be face of pipe
    //z axis will along pipe axis -> as a result 'z' value in this vector is 'x' speed in stepSpeed array

    float nextCartSpeed[param.numAxes];
    float v_dot;
    float v0_abs;
    float v1_abs;
    float junctSpeedScale;
    float* prevSpeed;
    float zeros[param.numAxes];

    for(int i = 0; i < param.numAxes; i++){
        zeros[i] = 0;
    }
    
    Move* prevMove = move.prevMove;

    if(prevMove->status != MOVE_STATUS_UNUSED){
        prevSpeed = zeros;
    } else {
        prevSpeed = prevMove->steadySpeed;
    }
    
    previousCartSpeed[0] = prevSpeed[1]*cos(pos[2])-pos[1]*prevSpeed[2]*sin(pos[2]);    //x_dot = r_dot*cos(theta)-r*theta_dot*sin(theta)
    previousCartSpeed[1] = prevSpeed[1]*sin(pos[2])+pos[1]*prevSpeed[2]*cos(pos[2]);    //y_dot = r_dot*sin(theta)+r*theta_dot*cos(theta)
    previousCartSpeed[2] = prevSpeed[0];                                                        //z_dot = axial speed of pipe
    
    nextCartSpeed[0] = move.steadySpeed[1]*cos(pos[2])-pos[1]*move.steadySpeed[2]*sin(pos[2]);    //x_dot = r_dot*cos(theta)-r*theta_dot*sin(theta)
    nextCartSpeed[1] = move.steadySpeed[1]*sin(pos[2])+pos[1]*move.steadySpeed[2]*cos(pos[2]);    //y_dot = r_dot*sin(theta)+r*theta_dot*cos(theta)
    nextCartSpeed[2] = move.steadySpeed[0];                                                    //z_dot = axial speed of pipe

    v_dot = (previousCartSpeed[0]*nextCartSpeed[0] + previousCartSpeed[1]*nextCartSpeed[1] + previousCartSpeed[2]*nextCartSpeed[2]);
    v0_abs = pow(previousCartSpeed[0],2) + pow(previousCartSpeed[1],2) + pow(previousCartSpeed[2],2);
    v1_abs = pow(nextCartSpeed[0],2) + pow(nextCartSpeed[1],2) + pow(nextCartSpeed[2],2);
    
    junctSpeedScale = v_dot/(v0_abs*v1_abs);         //Equals cos(theta)
    junctSpeedScale= sqrt((1 - junctSpeedScale)/2);      //Equals sin(theta/2)
    junctSpeedScale = param.maxTurnDelta*junctSpeedScale/(1-junctSpeedScale);  //Equals 'R' of virtual accleration curve
    junctSpeedScale = sqrt(param.maxCentripalAccel*junctSpeedScale);      //Equals max junction speed
    junctSpeedScale = junctSpeedScale/v0_abs;

    for(int i = 0; i < param.numAxes; i++){
      move.maxEntrySpeed[i] = prevSpeed[i]*junctSpeedScale;
    }

    if(move.maxEntrySpeed[0] > move.steadySpeed[0]){
        for(int i = 0; i < param.numAxes; i++){
            move.maxEntrySpeed[i] = move.steadySpeed[0];
        }
    }

    if(move.maxEntrySpeed[0] > prevSpeed[0]){
        for(int i = 0; i < param.numAxes; i++){
            move.maxEntrySpeed[i] = prevSpeed[0];
        }
    } 
}

void MovementPlanner::reserveMoves(Move &move){
    Move* curMove = &move;
    while(curMove->prevMove->status != MOVE_STATUS_UNUSED){   //Find first movement in buffer
        curMove = curMove->prevMove;
    }

    float duration = 0;
    int axis = 0;
    bool isEnd = false;

    while(duration < param.parseTime && !isEnd){
        curMove->status = 2;

        while(curMove->steadySpeed[axis] == 0 && axis < param.numAxes){     //Find first non-zero axis speed
            axis++;
        }

        if(axis != param.numAxes){
            duration += curMove->steadySpeed[axis] * curMove->numSteps[axis];
        }

        if(curMove->nextMove->status != MOVE_STATUS_UNUSED){
            curMove = curMove->nextMove;
        } else {
            isEnd = true;
        }
    }
}

void MovementPlanner::findJunctSpeeds(Move &move){
    bool isGreater;
    float* oldEndSpeeds;
    Move* curMove = &move;

    while(curMove->status == 1){   //Find last movement in buffer which is reserved
        if(curMove->prevMove->status != MOVE_STATUS_UNUSED){
            curMove = curMove->prevMove;
        }
    }

    if(curMove->nextMove->status != MOVE_STATUS_UNUSED){
        curMove = curMove->nextMove;    //Set current move to first editable move

        do {
            curMove->isNewlyOptimal = false;
            if(curMove->nextMove->status == MOVE_STATUS_UNUSED){
                for(int i=0; i<param.numAxes; i++){
                    curMove->entrySpeed[i] = sqrt(2*param.maxAccel[i]*curMove->numSteps[i]);
                }
            } else {
                for(int i=0; i<param.numAxes; i++){
                    curMove->entrySpeed[i] = sqrt(sq(curMove->nextMove->entrySpeed[i]) + 2*param.maxAccel[i]*curMove->numSteps[i]);
                }
            }
            
            isGreater = true;
            for(int i=0; i<param.numAxes; i++){
                if(curMove->entrySpeed[i] < curMove->maxEntrySpeed[i]){
                    isGreater = false;
                }
            }

            if(isGreater){
                curMove->entrySpeed = curMove->maxEntrySpeed;
                
                curMove->isNewlyOptimal = true;
                
                while(curMove->prevMove->status == MOVE_STATUS_EDITABLE){
                    curMove = curMove->prevMove;
                    curMove->isNewlyOptimal = true;
                }
            }
        } while(curMove->prevMove->status == MOVE_STATUS_EDITABLE);

        while(curMove->nextMove != MOVE_STATUS_UNUSED) {
            oldEndSpeeds = curMove->nextMove->entrySpeed;

            for(int i=0; i<param.numAxes; i++){
                curMove->nextMove->entrySpeed[i] = sqrt(sq(curMove->entrySpeed[i]) + 2*param.maxAccel[i]*curMove->numSteps[i]);
            }

            isGreater = true;
            for(int i=0; i<param.numAxes; i++){
                if(curMove->nextMove->entrySpeed[i] < oldEndSpeeds[i]){
                    isGreater = false;
                }
            }

            if(isGreater){
                curMove->nextMove->entrySpeed = oldEndSpeeds;
            } else if(curMove->isNewlyOptimal){ //Else->exit speed had to be reduced, not optimal
                curMove->isNewlyOptimal = false;
            }
        }
    }
}

void MovementPlanner::genMoveProfiles(Move &move){

    Move* curMove = &move;
    float currentDuration;
    float duration = 0;
    float lAxis = 0;    //limiting axis
    float topSpeed[param.numAxes];
    float leadInDur = 0;
    float leadOutDur = 0;
    float maxSpeedScale = 0;

    while(curMove->status == 1){   //Find last movement in buffer which is reserved
        if(curMove->prevMove != nullptr){
            curMove = curMove->prevMove;
        }
    }
    
    while(curMove->nextMove != nullptr){
        curMove = curMove->nextMove;

        if(!(curMove->isOptimal)){  //Calculate movement profiles for everything previously not optimal

            //First, determine the axis that can accelerate the slowest
            for(int j=0; j< param.numAxes; j++){
                currentDuration = (curMove->steadySpeed[j] - curMove->entrySpeed[j])/param.maxAccel[j];
                if(currentDuration > duration){
                    duration = currentDuration;  
                    lAxis = j;
                }
            }

            //For this axis, how many steps would it take to reach maximum speed, while still being able to reach exit junction speed?
            //NOT necessarily reach nominal speed; this may not be possible

            if(curMove->nextMove != nullptr){
                curMove->numStartSteps[lAxis] = (sq(curMove->entrySpeed[lAxis]) - sq(curMove->nextMove->entrySpeed[lAxis]) + 2*param.maxAccel[lAxis]*curMove->numSteps[lAxis])/(4*param.maxAccel[lAxis]);
            } else {  //Last item; end junction speed set to 0
                curMove->numStartSteps[lAxis] = (sq(curMove->entrySpeed[lAxis]) + 2*param.maxAccel[lAxis]*curMove->numSteps[lAxis])/(4*param.maxAccel[lAxis]);
            }

            //Find the maximum speed for the slowest accelerating axis
            topSpeed[lAxis] = sqrt(sq(curMove->entrySpeed[lAxis]) + 2*param.maxAccel[lAxis]*curMove->numStartSteps[lAxis]);

            //How long do we have for lead in/lead out?
            if(topSpeed[lAxis] > curMove->steadySpeed[lAxis]){
                leadInDur = (topSpeed[lAxis] - curMove->entrySpeed[lAxis])/param.maxAccel[lAxis];
                if(curMove->nextMove != nullptr){
                    leadOutDur = (curMove->nextMove->entrySpeed[lAxis] - topSpeed[lAxis])/param.maxAccel[lAxis];
                } else {
                    leadOutDur = (-1*topSpeed[lAxis])/param.maxAccel[lAxis];
                }
            } else {
                leadInDur = (curMove->steadySpeed[lAxis] - curMove->entrySpeed[lAxis])/param.maxAccel[lAxis];

                if(curMove->nextMove != nullptr){
                    leadOutDur = (curMove->nextMove->entrySpeed[lAxis] - curMove->steadySpeed[lAxis])/param.maxAccel[lAxis];
                } else {
                    leadOutDur = (-1*curMove->steadySpeed[lAxis])/param.maxAccel[lAxis];
                }
            }

            //What percent of our top speed can we reach?
            maxSpeedScale = topSpeed[lAxis]/curMove->steadySpeed[lAxis];

            //For each axis,
            for(int j=0; j<param.numAxes; j++){
                //Calculate the top speed we can reach
                if(j != lAxis)
                    topSpeed[j] = curMove->steadySpeed[j]*maxSpeedScale;

                //Calculate the number of lead-in steps
                curMove->numStartSteps = 0.5*(maxSpeedScale*curMove->steadySpeed[j] + curMove->entrySpeed[j])*leadInDur;

                //Calculate the number of lead-out steps,
                if(curMove->nextMove != nullptr){
                    curMove->numEndSteps[j] = 0.5*(maxSpeedScale*curMove->steadySpeed[j] + curMove->nextMove->entrySpeed[j])*leadInDur;
                } else {
                    curMove->numEndSteps[j] = 0.5*(maxSpeedScale*curMove->steadySpeed[j])*leadInDur;  //If we are on the last move, end at 0
                }

                //As a final check, in case of off-by-one errors, reduce the number of steps in lead-in and lead-out until they do not exceed total num steps
                while((curMove->numStartSteps[j] + curMove->numEndSteps[j]) > curMove->numSteps[j]){
                    curMove->numStartSteps[j]--;

                    if((curMove->numStartSteps[j] + curMove->numEndSteps[j]) > curMove->numSteps[j]){
                    curMove->numEndSteps[j]--;
                    }
                }

                //Any remaining steps occur at a steady maximum speed
                curMove->numSteadySteps[j] = curMove->numSteps[j] - curMove->numStartSteps[j] - curMove->numEndSteps[j];

                //Generate the lead-in delays
                genAccelDelays(curMove->entrySpeed, topSpeed[j], curMove->startAccel, curMove->numStartSteps);

                //Generate the lead-out delays
                if(curMove->nextMove != nullptr){
                    genAccelDelays(curMove->nextMove->entrySpeed, topSpeed[j], curMove->endAccel, curMove->numEndSteps);
                } else {
                    int zeros[param.numAxes] = {0,0,0};
                    genAccelDelays(zeros, topSpeed[j], curMove->endAccel, curMove->numEndSteps);
                }
            }
            
        }
        curMove->isOptimal = curMove->isNewlyOptimal;
    }
}

void MovementPlanner::genAccelDelays(float startSpeed[param.numAxes], float endSpeed[param.numAxes], float accelDelays[param.numAxes][param.maxAccelSteps], int numSteps[param.numAxes]){
    float speed1;
    float speed2;
    int isAccel;    //Is the axis accelerating (or decelerating)

    for(int i = 0; i < param.numAxes; i++){
        if(endSpeed[0] >= startSpeed[0]){
            isAccel = 1;
        } else {
            isAccel = -1;
        }

        numSteps[i] = 0;
        speed1 = startSpeed[i];
        accelDelays[i][numSteps[i]] = 1/speed1;

        while(speed1 != endSpeed[i]){
            numSteps[i]++;
            
            speed2 = speed1*abs(speed1);
            speed2 += 2*param.maxAccel[i]*isAccel;
            speed2 = sqrt(abs(speed2)) * ((speed2 > 0) ? 1 : ((speed2 < 0) ? -1 : 0));  //Take sqaure root, but maintain sign

            if(speed2*isAccel > endSpeed[i]*isAccel){
            speed2 = endSpeed[i];
            }

            accelDelays[i][numSteps[i]] = 1/speed2;
            
            speed1 = speed2;
        }
    }
}

void MovementPlanner::resetPos(){
    for(int i = 0; i < param.numAxes; i++){
        pos[i] = param.zeroPos[i];
    }
}