#ifndef Main_h
#define Main_h
#endif

#define X_STEP_PIN 2
#define Z_STEP_PIN 4
#define A_STEP_PIN 3

#define X_DIR_PIN 5
#define Z_DIR_PIN 7
#define A_DIR_PIN 6

#define ENABLE_PIN 8
#define MAX_BUF 64

#define INTERRUPT_PIN 9 //CHANGE PIN

#define X_LIM 10        //CHANGE PIN
#define Z_LIM 11        //CHANGE PIN
#define A_LIM 12        //CHANGE PIN

#define bufSize 4

class Pin
{
  public:
    int stepPin[3] = {X_STEP_PIN, Z_STEP_PIN, A_STEP_PIN};
    int dirPin[3] = {X_DIR_PIN, Z_DIR_PIN, A_DIR_PIN};
    int limPin[3] = {X_LIM, Z_LIM, A_LIM};
};

const Pin pin;

extern void readLimSwitches(bool (&) [NUM_AXIES]);
extern void stepMotor(int);
extern void setDir(bool, int);
extern int remainingSteps[NUM_AXIES];
//extern float stepDelay[NUM_AXIES][MAX_ACCEL_STEPS];
extern void setInterrupt(int);
