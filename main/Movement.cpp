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
float max_step_accel[NUM_AXIES] = {X_MAX_ACCEL, A_MAX_ACCEL, Z_MAX_ACCEL};
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
  
  stepDelay[axis][0] = 1000000/(stepsPer[axis]*axisSpeed);
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

void *getCmdSpeed(GCodeCommand* g, float (&axisSpeed)[NUM_AXIES], float (&pos)[NUM_AXIES], bool absMode){
  bool hasFeedrate;
  float feedrate = g->getParameter('F',hasFeedrate);
  if(!hasFeedrate){
    feedrate = MIN_FEEDRATE;
  }

  float val;
  float del[NUM_AXIES];
  
  for(int i = 0; i < NUM_AXIES; i++){
    val = g->getParameter(axies.charAt(i),hasParam[i]);
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

  getSpeed(feedrate, pos[1], del, axisSpeed);
  return axisSpeed;
}

float *getAccelToSpeed(float (currentSpeed)[NUM_AXIES], float (nextSpeed)[NUM_AXIES], float (&accelToSpeed)[NUM_AXIES]){
  
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

        if(totalAcceleration[i] > max_step_accel[i]){
            exceedsMaxAccel = true;
        }
        //If moving from current speed to next speed in one time exceeds acceleration limits for any axis,
        //set this boolean true
    }

    if (exceedsMaxAccel){
        for (int i = 0; i < NUM_AXIES; i++){
            if(totalAcceleration[i] != 0){
                minDecelForAxis = sqrt(abs(max_step_accel[NUM_AXIES]/totalAcceleration[i]));
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
    
    return accelToSpeed;
}

bool isFinishedMove(){ 
    for(int i = 0; i < NUM_AXIES; i++){
      if(remainingSteps[i] > 0 && hasParam[i]){
        return false;
      }
    }

    return true;
}
