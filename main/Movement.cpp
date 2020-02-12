#ifndef Movement_h
#include "Movement.h"
#endif

#ifndef GCodeCommand_h
#include "GCodeCommand.h"
#endif

#ifndef Main_h
#include "main.h"
#endif

float stepsPer[NUM_AXIES] = {X_STEPS_PER_MM, Z_STEPS_PER_MM, A_STEPS_PER_RAD};
float maxAccel[NUM_AXIES] = {X_MAX_ACCEL, A_MAX_ACCEL, Z_MAX_ACCEL};
bool hasParam[NUM_AXIES];

void homeAxes(){
  bool limSwitch[NUM_AXIES];
  bool isDone = false;

  while(!isDone){
    readLimSwitches(limSwitch);
    
    isDone = true;
    for(int i=0; i < NUM_AXIES; i++){
      if(limSwitch[i]){
        stepMotor(i);
        isDone = false;
      }
    }

    delay(10);
  }
}

void moveMotor(int axis, float distance, float axisSpeed, bool (&isSteady) [NUM_AXIES]){
  if(distance < 0){
    setDir(true, axis);
    distance = -distance;
  } else {
    setDir(false, axis);
  }
  int numSteps = ceil(distance*stepsPer[axis]);
  remainingSteps[axis] = numSteps;
  isSteady[axis] = true;
  
  //stepDelay[axis][0] = 1000000/(stepsPer[axis]*axisSpeed);
  setInterrupt(axis);
};

void getSpeed(float fr, float zValStart, float delta[NUM_AXIES], float (&v)[NUM_AXIES])     //Takes feedrate & array of z, a, x, delta values
                                                                                            //Returns array of speeds for z, a, x
{   
  float r1;
  float r2;
  
  float x_speed;
  float z_speed;
  float a_speed;
  
  float delta_x = delta[0];
  float delta_z = delta[1];
  float delta_a = delta[2];
  
  float r_avg = (delta_z+2*zValStart)/2;
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
  v[0] = abs(x_speed);
  v[1] = abs(z_speed);
  v[2] = abs(a_speed);
}
/*
bool executeCommand(GCodeCommand* (CodeBuf)[2], float (&pos)[NUM_AXIES], bool absMode, bool (&isSteady) [NUM_AXIES]){
  bool isMoving = false;
  bool hasFeedrate = false;
  float velocity[NUM_AXIES];

  for(int i = 0; i < NUM_AXIES; i++){
    hasParam[i] = false;
  }

  float val;
  float del[NUM_AXIES];
  
  float feedrate = CodeBuf[0]->getParameter('F',hasFeedrate);
  if(!hasFeedrate){
    feedrate = MIN_FEEDRATE;
  }   //NOTE -> to be read from speed array
  
  for(int i = 0; i < NUM_AXIES; i++){
    val = CodeBuf[0]->getParameter(axies.charAt(i),hasParam[i]);
    if(hasParam[i]){
      if(absMode){
        del[i] = val - pos[i];
        pos[i] = val;
      } else {
        del[i] = val;
        pos[i] = pos[i] + val;
      }
    } else {
      del[i] = 0;
    }
  }
  
  getSpeed(feedrate, pos[1], del, velocity);

  for(int i = 0; i < NUM_AXIES; i++){
    if(del[i] != 0){
      isMoving = true;
      moveMotor(i, del[i], velocity[i], isSteady);
    }
  }

  return isMoving;
}
*/
void getNumSteps(GCodeCommand* g, bool absMode, int (&pos)[NUM_AXIES], int* numSteps){
  float val;
  
  for(int i = 0; i < NUM_AXIES; i++){
    val = g->getParameter(axies.charAt(i),hasParam[i]);
    if(hasParam[i]){
      if(absMode){
        numSteps[i] = round(val*stepsPer[i]) - pos[i];
        pos[i] = numSteps[i];
      } else {
        numSteps[i] = round(val*stepsPer[i]);
        pos[i] = numSteps[i] + val;
      }
    } else {
      numSteps[i] = 0;
    }
  }
}


void getCmdSpeed(GCodeCommand* g, float (&axisSpeed)[NUM_AXIES], int (&pos)[NUM_AXIES], int* numSteps){
  bool hasFeedrate;
  float feedrate = g->getParameter('F',hasFeedrate);
  if(!hasFeedrate){
    feedrate = MIN_FEEDRATE;
  }

  float val;
  float del[NUM_AXIES];

  for(int i = 0; i < NUM_AXIES; i++){
    del[i] = numSteps[i] / stepsPer[i];
  }
  
  getSpeed(feedrate, pos[1], del, axisSpeed);
  return;
}

void getAccelToSpeed(float (currentSpeed)[NUM_AXIES], float (nextSpeed)[NUM_AXIES], float* accelToSpeed){
  
    float inverseTimeDelay[NUM_AXIES];
    
    float totalAcceleration[NUM_AXIES];
    float minDecelForAxis;
    float decel = 1;
    bool exceedsMaxAccel;

    float current_step_speed[NUM_AXIES];
    float next_step_speed[NUM_AXIES];
    
    for(int i = 0; i < NUM_AXIES; i++){
      current_step_speed[i] = currentSpeed[i] * stepsPer[i];
      next_step_speed[i] = nextSpeed[i] * stepsPer[i];
    }
    
    exceedsMaxAccel = false;

    for (int i = 0; i < NUM_AXIES; i++){
        if(current_step_speed[i] != 0){
            inverseTimeDelay[i] = current_step_speed[i];
        } else {
            inverseTimeDelay[i] = next_step_speed[i];
        }
        //Estimate inverse of time to accelerate from current speed to next speed

        totalAcceleration[i] = (next_step_speed[i] - current_step_speed[i])*inverseTimeDelay[i];
        //Calculate total acceleration is speed change in single steps_per (delta v/ delta t)

        if(totalAcceleration[i] > maxAccel[i]){
            exceedsMaxAccel = true;
        }
        //If moving from current speed to next speed in one time exceeds acceleration limits for any axis,
        //set this boolean true
    }

    if (exceedsMaxAccel){
        for (int i = 0; i < NUM_AXIES; i++){
            if(totalAcceleration[i] != 0){
                minDecelForAxis = sqrt(abs(maxAccel[NUM_AXIES]/totalAcceleration[i]));
                if(minDecelForAxis<decel){
                    decel = minDecelForAxis;
                }
            }
        }
    }   //Calculate scale factor for each axis to maintain acceleration requirements.
        //Select minimum scale factor of every axis to return

    for(int i = 0; i < NUM_AXIES; i++){
      accelToSpeed[i] = decel * currentSpeed[i];
    }
    
    return;
}


void genAccelDelays(float* startSpeed, float* endSpeed, float (*accelDelays)[MAX_ACCEL_STEPS], int* numStep){
  float speed1;
  float speed2;
  int isAccel;    //Is the axis accelerating (or decelerating)
  
  for(int i = 0; i < NUM_AXIES; i++){
    if(endSpeed[0] >= startSpeed[0]){
      isAccel = 1;
    } else {
      isAccel = -1;
    }
    
    numStep[i] = 0;
    speed1 = startSpeed[i];
    accelDelays[i][numStep[i]] = 1/speed1;
    
    while(speed1 != endSpeed[i]){
      numStep[i]++;
      
      speed2 = speed1*abs(speed1);
      speed2 += 2*maxAccel[i]*isAccel*(1/stepsPer[i]);
      speed2 = sqrt(abs(speed2)) * ((speed2 > 0) ? 1 : ((speed2 < 0) ? -1 : 0));  //Take sqaure root, but maintain sign

      if(speed2*isAccel > endSpeed[i]*isAccel){
        speed2 = endSpeed[i];
      }

      accelDelays[i][numStep[i]] = 1/speed2;
      
      speed1 = speed2;
    }
  }
}

bool getStepsToAccel(float junctSpeed[NUM_AXIES], float nominalSpeed[NUM_AXIES], float &duration, int (&numSteps)[NUM_AXIES]){
    if(speedScale < 0 || speedScale >= 1){
        return false;
    }

    /*
    float absNominalSpeed[NUM_AXIES];

    for(int i=0; i < NUM_AXIES; i++){
      if(nominalSpeed[NUM_AXIES] < 0){
        absNominalSpeed[i] = -1*nominalSpeed[i];
      } else {
        absNominalSpeed[i] = nominalSpeed[i];
      }
    }
    */
    
    duration = 0;
    float currentDuration;
    float reducedSpeed[NUM_AXIES];
    
    for(int i=0; i< NUM_AXIES; i++){
        currentDuration = (nominalSpeed[i]-junctSpeed[i])/maxAccel[i];
        if(currentDuration > duration){
            duration = currentDuration;  
        }
    }
    
    for(int i=0; i< NUM_AXIES; i++){
        numSteps[i] = ceil((nominalSpeed[i]+reducedSpeed[i])*duration/2)+1;
    }
    
    return true;
}

bool getAccelDelays(float junctSpeed[NUM_AXIES], float nominalSpeed[NUM_AXIES], int numSteps[NUM_AXIES], bool (&dir)[NUM_AXIES], float duration, bool isAccel, float (*delay)[MAX_ACCEL_STEPS]){
    
    
    for(int i = 0; i < NUM_AXIES; i++){
        if(numSteps[i] > MAX_ACCEL_STEPS){
            return false;
        }
    }
    
    float vS[NUM_AXIES];        //Velocity at t=T/2 in s-curve
    float aO[NUM_AXIES];        //Overall acceleration. Accel required to transition between speeds LINEARLY in given duration
    float aS[NUM_AXIES];        //Max acceleration in s-curve. Occurs at t=T/2
    float jerk[NUM_AXIES];      //Rate of change of acceleration
    float c1[NUM_AXIES];        //Constant for solving inverse cubic to solve for first delay
    float c2;                   //Another constant for solving inverse cubic
    float nominalSpeedRatio;    //Ratio of nominal speed to nominal speed of 0th axis
    float stepSpeed;
    int startStep[NUM_AXIES];
    
    for(int i = 0; i < NUM_AXIES; i++){
        if(nominalSpeed[i] < 0){
          nominalSpeed[i] = -1*nominalSpeed[i];
          dir[i] = false;
        } else {
          dir[i] = true;
        }
        
        if(isAccel){
            startStep[i] = 0;
        } else {
            startStep[i] = numSteps[i] - 1;
        }
    }
    
    aO[0] = 2*(nominalSpeed[0]-junctSpeed[0])/duration;
    jerk[0] = 2*aO[0]/duration;
    c1[0] = -1/(jerk[0]/3);
    c2 = junctSpeed[0]/(jerk[0]/2);
    delay[0][startStep[0]] = cbrt(-1*c1[0]+sqrt(pow(c1[0],2)+pow(c2,3)))+cbrt(-1*c1[0]-sqrt(pow(c1[0],2)+pow(c2,3))); //Solving inverse cube to determine first delay
    vS[0] = junctSpeed[0]+(jerk[0]*pow(duration,2))/8;
    aS[0] = jerk[0]*duration/2;
    
    if(NUM_AXIES>0){  //Rather than solving using the equations above, we can scale them based on the ratio of maximum velocities
        for(int i=1; i< NUM_AXIES; i++){
            nominalSpeedRatio = nominalSpeed[i]/nominalSpeed[0];
            aO[i] = aO[0]*nominalSpeedRatio;
            jerk[i] = jerk[0]*nominalSpeedRatio;
            c1[i] = c1[0]/nominalSpeedRatio;
            delay[i][startStep[i]] = pow((-c1[i]+sqrt(pow(c1[i],2)+pow(c2,3))),1/3)+pow((-c1[i]-sqrt(pow(c1[i],2)+pow(c2,3))),1/3); //There might be a function to determine this based on delay[0][startStep[0]] & nominalSpeedRatio, but idk
            vS[i] = vS[0]*nominalSpeedRatio;
            aS[i] = aS[0]*nominalSpeedRatio;
        }
    }
    
    int stepNum;
    float elapsedTime;
    
    for(int i=0; i < NUM_AXIES; i++){
        
        elapsedTime = delay[i][startStep[i]];
        if(isAccel){
            stepNum = startStep[i] + 1;
        } else {
            stepNum = startStep[i] - 1;
        }
        
        while(elapsedTime < (duration/2)){    //Increasing accel portion of s-curve
            stepSpeed = junctSpeed[i]+jerk[i]*pow((elapsedTime),2)/2;
            delay[i][stepNum] = 1/stepSpeed;
            elapsedTime += delay[i][stepNum];
            if(isAccel){
                stepNum++;
            } else {
                stepNum--;
            }
        }
        
        while(elapsedTime < (duration)){    //Decreasing accel portion of s-curve
            stepSpeed = -1*(jerk[i]*pow((elapsedTime-duration/2),2))/2 + aS[i]*(elapsedTime-duration/2)+vS[i];
            delay[i][stepNum] = 1/stepSpeed;
            elapsedTime += delay[i][stepNum];
            if(isAccel){
                stepNum++;
            } else {
                stepNum--;
            }
        }
        
        while(((stepNum < numSteps[i]) && isAccel) || ((stepNum >= 0) && !isAccel)){    //Since numSteps is estimated based on duration, may overestimate by 1 or 2 steps. Continue at nominalSpeed until numSteps is reached
            delay[i][stepNum] = 1/nominalSpeed[i];
            if(isAccel){
                stepNum++;
            } else {
                stepNum--;
            }
        }
    }
    
    return true;
}

void getMaxJunctScale(float junctSpeed[NUM_AXIES], float previousSpeed[NUM_AXIES], float nextSpeed[NUM_AXIES], int pos[NUM_AXIES]){

    float previousCartSpeed[NUM_AXIES]; //Cartesian step speed
    //We will define x-y plane to be face of pipe
    //z axis will along pipe axis -> as a result 'z' value in this vector is 'x' speed in stepSpeed array
    float nextCartSpeed[NUM_AXIES];
    float v_dot;
    float v0_abs;
    float v1_abs;
    float junctSpeedScale;
    
    previousCartSpeed[0] = previousSpeed[1]*cos(pos[2])-pos[1]*previousSpeed[2]*sin(pos[2]);    //x_dot = r_dot*cos(theta)-r*theta_dot*sin(theta)
    previousCartSpeed[1] = previousSpeed[1]*sin(pos[2])+pos[1]*previousSpeed[2]*cos(pos[2]);    //y_dot = r_dot*sin(theta)+r*theta_dot*cos(theta)
    previousCartSpeed[2] = previousSpeed[0];                                                        //z_dot = axial speed of pipe
    
    nextCartSpeed[0] = nextSpeed[1]*cos(pos[2])-pos[1]*nextSpeed[2]*sin(pos[2]);    //x_dot = r_dot*cos(theta)-r*theta_dot*sin(theta)
    nextCartSpeed[1] = nextSpeed[1]*sin(pos[2])+pos[1]*nextSpeed[2]*cos(pos[2]);    //y_dot = r_dot*sin(theta)+r*theta_dot*cos(theta)
    nextCartSpeed[2] = nextSpeed[0];                                                    //z_dot = axial speed of pipe

    v_dot = (previousCartSpeed[0]*nextCartSpeed[0] + previousCartSpeed[1]*nextCartSpeed[1] + previousCartSpeed[2]*nextCartSpeed[2]);
    v0_abs = pow(previousCartSpeed[0],2) + pow(previousCartSpeed[1],2) + pow(previousCartSpeed[2],2);
    v1_abs = pow(nextCartSpeed[0],2) + pow(nextCartSpeed[1],2) + pow(nextCartSpeed[2],2);
    
    junctSpeedScale = v_dot/(v0_abs*v1_abs);         //Equals cos(theta)
    junctSpeedScale= sqrt((1 - junctSpeedScale)/2);      //Equals sin(theta/2)
    junctSpeedScale = MAX_TURN_DELTA*junctSpeedScale/(1-junctSpeedScale);  //Equals 'R' of virtual accleration curve
    junctSpeedScale = sqrt(MAX_CENTRIPAL_ACCEL*junctSpeedScale);      //Equals max junction speed
    junctSpeedScale = junctSpeedScale/v0_abs;

    for(int i = 0; i < NUM_AXIES; i++){
      junctSpeed[i] = previousSpeed[i]*junctSpeedScale;
    }
}

void getMaxAccelToSpeed(float entrySpeed[NUM_AXIES], float exitSpeed[NUM_AXIES], int numSteps[NUM_AXIES]){
  for(int i=0; i<NUM_AXIES; i++){
    maxExitSpeed = sqrt(sq(entrySpeed[i]) + 2*maxAccel[i]*numSteps[i]);
  }
}

//Returns true if every value of speed1 is greater than or equal to respective speed2 values
bool compareSpeeds(float speed1[NUM_AXIES], float speed2[NUM_AXIES]){
  int isGreater;
  for(int i=0; i<NUM_AXIES; i++){
    if(speed1[i] < speed2[i]){
      isGreater = 0;
    } else if(speed1[i] > speed2[i]){
      isGreater 
    }
  }
}

bool isFinishedMove(){ 
    for(int i = 0; i < NUM_AXIES; i++){
      if(remainingSteps[i] > 0 && hasParam[i]){
        return false;
      }
    }

    return true;
}
