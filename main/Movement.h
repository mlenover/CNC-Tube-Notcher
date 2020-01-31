#ifndef Movement_h
#define Movement_h
#endif

#ifndef GCodeCommand_h
#include "GCodeCommand.h"
#endif

#ifndef Arduino_h
#include <Arduino.h>
#endif

#define VERSION              (2)  // firmware version
#define NUM_AXIES 3
const String axies = "XZA";

#define X_STEPS_PER_MM 100
#define Z_STEPS_PER_MM 300
#define A_STEPS_PER_RAD 95.4929658551

#define X_MAX_SPEED 10 //Max speed of x axis axis in mm/s
#define Z_MAX_SPEED 10 //Max speed of plunge axis in mm/s
#define A_MAX_SPEED 10 //Max speed of rotation axis in rad/s

#define X_MAX_ACCEL 10 //Max acceleration of x axis in mm/s^2
#define Z_MAX_ACCEL 10 //Max acceleration of plunge axis in mm/s^2
#define A_MAX_ACCEL 10 //Max acceleration of rotation axis in rad/s^2

#define MAX_FEEDRATE         1000000
#define MIN_FEEDRATE         1

#define MAX_ACCEL_STEPS 2000

#define X_BOUNDS 350
#define Z_BOUNDS 400
#define A_BOUNDS 7

#define MAX_TURN_DELTA 0.1
#define MAX_CENTRIPAL_ACCEL 2

extern void homeAxes();
extern void getSpeed(float, float, float [NUM_AXIES], float (&) [NUM_AXIES]);
extern bool executeCommand(GCodeCommand* [2], float (&) [NUM_AXIES], bool, bool (&)[NUM_AXIES]);
extern bool isFinishedMove();
//extern float calcDecel(float [NUM_AXIES], float [NUM_AXIES], float (&) []NUM_AXIES, int);
extern void getCmdSpeed(GCodeCommand*, float (&)[NUM_AXIES], int (&)[NUM_AXIES], int*);
extern void getAccelToSpeed(float [NUM_AXIES], float [NUM_AXIES], float*);
extern void getNumSteps(GCodeCommand* g, bool absMode, int (&)[NUM_AXIES], int*);
extern bool getStepsToAccel(float, float*, float, int*);
extern bool getAccelDelays(float, float*, int*, bool*, float, bool, float* (*));
extern void getMaxJunctScale(float, float, float*, float*, int*);
extern void getMaxAccelToSpeed(float, float(&), float*, int*);

class Move {
    public:
        float entryScale = 0;                   //% of steady speed to begin movement at
        float steadySpeed[NUM_AXIES];           //Commanded speed for each axis
        float exitScale = 0;                    //% of steady speed to end movement at
        float maxEntryScale = 0;                //Max value of entryScale based on centripital acceleration thru movement change
        float maxExitScale = 0;                 //Max value of exitScale based on centripital acceleration thru movement change
        float (*startAccel)[MAX_ACCEL_STEPS];   //Array of delays for lead-in acceleration
        float (*endAccel)[MAX_ACCEL_STEPS];     //Array of delays for lead-out acceleration
        int* numSteps;                          //Total number of steps for movement
        int* numStartSteps;                     //Number of steps in lead-in acceleration
        int* numSteadySteps;                    //Number of steps steady-state movement
        int* numEndSteps;                       //Number of steps in lead-out acceleration
        int* startStep;                         //Step position at beginning of movement
};
