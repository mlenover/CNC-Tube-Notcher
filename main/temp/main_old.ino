#ifndef _INCL_MAIN
#include <main.h>
#endif

#include <commands.h>

//FOR TESTINGS VALUES
float r1 = 0;
float r2 = 3;
float theta1 = 0;
float theta2 = 0;
float z1 = 0;
float z2 = 2;
float feedrate = 100;
//END OF TESTING VALUES

int i;

float delta_x = z2 - z1;
float delta_z = r2 - r1;
float delta_a = theta2 - theta1;

float delta[3] = {delta_z, delta_a, delta_x};

float z_speed;
float a_speed;
float x_speed;

float previous_speed[NUM_AXIES];
float current_speed[NUM_AXIES];
float next_speed[NUM_AXIES];

float steps_per[NUM_AXIES] = {Z_STEPS_PER_MM, A_STEPS_PER_RAD, X_STEPS_PER_MM};
float max_step_accel[NUM_AXIES] = {Z_MAX_ACCEL*Z_STEPS_PER_MM, A_MAX_ACCEL*A_STEPS_PER_RAD, X_MAX_ACCEL*X_STEPS_PER_MM};

float stepPosition[NUM_AXIES] = {0, 0, 0};

float getNextCommand(){

}

void stepMotor(int motorNum, bool dir){

}
/*Splits feedrate into speed of each axis
@return speed of x,z,a axies
@input feedrate from gcode command
@input change in commanded position*/
float *getSpeed(float feedrate, float delta[NUM_AXIES], float speed[NUM_AXIES])
{
    delta_z = delta[1];
    delta_a = delta[2];
    delta_x = delta[3];

    float r_avg = (r2+r1)/2;
    if (delta_z != 0) {
        z_speed = feedrate / sqrt(sq(delta_x/delta_z) + sq(r_avg)*sq(delta_a/delta_z) + 1);
        a_speed = (delta_a / delta_z)*z_speed;
        x_speed = (delta_x / delta_z)*z_speed;
    }
    else if (delta_a != 0){
        z_speed = 0;
        a_speed = feedrate / sqrt(sq(r_avg) + sq(delta_x/delta_a));
        x_speed = (delta_x / delta_a) * a_speed;
    }
    else {
        z_speed = 0;
        a_speed = 0;
        x_speed = feedrate;
    }
    speed[1] = abs(z_speed);
    speed[2] = abs(a_speed);
    speed[3] = abs(x_speed);
    return speed;
}

/*Calculates how much speeds must be slowed such that transisitioning from
current_speed * decel to next_speed * decel does not 
exceed max acceleration for any axis
@input current speed in step/s
@input next speed in steps/s
@input max acceleration for each axis
@return scale factor for current & next speeds
*/
float calcDecel(float current_step_speed[NUM_AXIES], float next_step_speed[NUM_AXIES], float max_step_accel[NUM_AXIES])
{

    float inverseTimeDelay[NUM_AXIES];
    float totalAcceleration[NUM_AXIES];
    float minDecelForAxis;
    float decel = 1;
    bool exceedsMaxAccel;

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

    return decel;
}

/*Find axis that must slow down the most in order meet max acceleration
@input current speed in step/s
@input next speed in steps/s
@input max acceleration for each axis
@output integer of axis that is limiting
*/
int findLimitingAxis(float current_step_speed[NUM_AXIES], float next_step_speed[NUM_AXIES], float max_step_accel[NUM_AXIES]){
    long difference = 0;        //To store difference between max acceleration for single axis
                                //and theoretical acceleration transitioning from current speed to next speed in single step

    long maxDifference = 0;     //Stores max values of above value

    int limitingAxis = 0;       //Stores axis for which max difference value is greatest
                                //will limit acceleration of all axies

    for (int i=0; i < NUM_AXIES; i++){
        difference = abs(current_step_speed[i]*(current_step_speed[i]-next_step_speed[i]));
        if (difference > maxDifference){
            maxDifference = difference;
            limitingAxis = i;
        }
    }

    return limitingAxis;
}

/*
Calculates number of steps for each axis to accelerate from current speed to next speed
given the maximum acceleration of the limiting axis. Velocity vectors MUST be colinear.
@input current speed in step/s
@input next speed in steps/s
@input max acceleration for each axis
@input integer of axis that is limiting
@output number of steps to accelerate
*/
int *stepsToAccelerate(float current_step_speed[NUM_AXIES], float next_step_speed[NUM_AXIES], float max_step_accel[NUM_AXIES], int limitingAxis, int numSteps[NUM_AXIES]){

    numSteps = 0;
    float numStepsFactor;

    numSteps[limitingAxis] = ceil(abs(sq(current_step_speed[limitingAxis]) - sq(next_step_speed[limitingAxis]))/(2*max_step_accel[i]));
    
    numStepsFactor = numSteps[limitingAxis]/(next_step_speed[limitingAxis]+current_step_speed[limitingAxis]);

    for (int i = 0; i < NUM_AXIES; i++){
        if(i != limitingAxis){
            numSteps[i] = ceil((next_step_speed[i]+current_step_speed[i])*numStepsFactor);
        }
    }
    
    return numSteps;
}

bool checkNumSteps(int newPos[NUM_AXIES], int numStepsStart[NUM_AXIES], int numStepEnd[NUM_AXIES]){
    bool canReachFeedrate = true;
    for (int i = 0; i < NUM_AXIES; i++){
        if ((numStepsStart[i] + numStepsStart[i]) > newPos[i]){
            canReachFeedrate = false;
        }
    }

    return canReachFeedrate;
}

/*
Accelerates all axies together based on maximum acceleration of provided limiting axis
Will terminate when maxSteps is reached on every axis. Updates next_step_speed array to final velocity of 
each axis if maxSteps is reached
@input previous speed in step/s
@input next speed in steps/s
@input max acceleration for each axis
@input max number of steps in each axis
@output updates next speed array to final speed reached if num_steps is reached on any axis
*/

float *accelLinear(float previous_step_speed[NUM_AXIES], float next_step_speed[NUM_AXIES], float max_step_accel[NUM_AXIES], int limitingAxis, int finalNumSteps[NUM_AXIES])
{
    bool metMaxSteps = false;
    bool reachedNextSpeed = false;

    int numSteps[NUM_AXIES] = {0, 0, 0};
    int totalSteps = 0;
    float setPos;
    float posDifference;

    float accel[NUM_AXIES];
    float t;

    unsigned long startTime;
    unsigned long currentTime;
    unsigned long elapsedTime;

    accel[limitingAxis] = max_step_accel[limitingAxis];

    t = abs((next_step_speed[limitingAxis] - previous_step_speed[limitingAxis])/max_step_accel[limitingAxis]); //Calculate theoretical time to completion if final velocity is reached

    for (int i = 0; i < NUM_AXIES; i++){    //calculate acceleration for remaining 2 axies, assuming time to completion is the same (synchronize axies)
        if(i != limitingAxis){
            accel[i] = (next_step_speed[i]-previous_step_speed[i])/t;
        }
    }

    startTime = micros();                                   //Start measuring time from beginning of movement
    while(!metMaxSteps){            //Check if EITHER maxSteps is reached or final velocity is reached
        for (int i = 0; i < NUM_AXIES; i++){                //For each axis,

            currentTime = micros();                         
            elapsedTime = (currentTime-startTime)/1000000;  //Calculate elapsed time since start

            setPos = previous_step_speed[i]*elapsedTime + (1/2)*accel[i]*sq(elapsedTime);
            posDifference = setPos - numSteps[i];

            while(abs(posDifference) > 0.5 && numSteps[i] < finalNumSteps[i]){              //If position of motor is more than 1/2 step away from expected position (based on acceleration and elapsed time),
                if(posDifference > 0){                                                      //check if required position is greater than or less than current position
                    stepPosition[i] = stepPosition[i] + 1;                                  //increment or decrement step counter, and step motor in the correct direction
                    stepMotor(i,1);
                } else {
                    stepPosition[i] = stepPosition[i] - 1;
                    stepMotor(i,0);
                }
                numSteps[i]++;                                                              //Count every time any motor makes a step
            }
            
        }

        metMaxSteps = true;
        for (int i = 0; i < NUM_AXIES; i++){
            if(numSteps[i] >= finalNumSteps[i]){                                            //Check if total number of steps was exceeded
                metMaxSteps = false;
            }
        }
    }

    currentTime = micros(); 
    elapsedTime = (currentTime-startTime)/1000000;  //Calculate elapsed time since start

    if(elapsedTime < t){                        //If max number of steps was reached before velocity,
        for (int i = 0; i < NUM_AXIES; i++){    //set final velocity vector to final speed of each axis
            next_step_speed[i] = previous_step_speed[i] + accel[i]*elapsedTime;
        }
    }
    return next_step_speed;
}

/*Moves all axies together until endStep is reached
@input speed of each axis
@input position of each axis to finish moving
*/

void moveLinear(long speed[NUM_AXIES], long endStep[NUM_AXIES]){

    float numSteps[NUM_AXIES] = {0, 0, 0};
    bool metEndStep = false;

    unsigned long startTime;
    unsigned long currentTime;
    unsigned long elapsedTime;

    float setPos;
    float posDifference;

    startTime = micros();

    while(!metEndStep){
        for (int i = 0; i < NUM_AXIES; i++){
            currentTime = micros();                         
            elapsedTime = (currentTime-startTime)/1000000;  //Calculate elapsed time since start

            setPos = speed[i]*elapsedTime;
            posDifference = setPos - numSteps[i];

            while(abs(posDifference) > 0.5 && stepPosition[i] != endStep[i]){   //If position of motor is more than 1/2 step away from expected position (based on velocity and elapsed time),
                if(posDifference > 0){                      //check if required position is greater than or less than current position
                    stepPosition[i] = stepPosition[i] + 1;  //increment or decrement step counter, and step motor in the correct direction
                    stepMotor(i,1);
                } else {
                    stepPosition[i] = stepPosition[i] - 1;
                    stepMotor(i,0);
                }
                numSteps[i]++;                          //Count every time any motor makes a step
            }

            metEndStep = true;                          //assume each axis has reached the end step location
            for (int i = 0; i < NUM_AXIES; i++){
                if(stepPosition[i] != endStep[i]){      //if any have NOT, set this condition back to false                                  
                    metEndStep = false;
                }
            }
        }
    }
}
void setup()
{
    Serial.begin(BAUDRATE);
}

void loop()
{   
    getNextCommand();
    int i = 1;
}