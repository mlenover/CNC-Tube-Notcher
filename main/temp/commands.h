#ifndef _INCL_COMMANDS
#define _INCL_COMMANDS
#endif

#define BAUDRATE                 (57600)  // How fast is the Arduino talking?
#define MAX_BUF              (64)  // What is the longest message Arduino can store?
#define NUM_AXIES 3

//------------------------------------------------------------------------------
// STRUCTS
//------------------------------------------------------------------------------
// for line()

/*
typedef struct {
  long delta;  // number of steps to move
  long absdelta;
  long over;  // for dx/dy bresenham calculations
} Axis;
*/

/*
typedef struct {
  int step_pin;
  int dir_pin;
  int enable_pin;
  int limit_switch_pin;
} Motor;
*/