#include "main.h"

HardwareTimer *xAxisTim = new HardwareTimer(TIM1);
HardwareTimer *zAxisTim = new HardwareTimer(TIM2);
HardwareTimer *aAxisTim = new HardwareTimer(TIM3);

HardwareTimer *Tim[param.numAxes] = {xAxisTim, zAxisTim, aAxisTim};

void xAxisInterrupt(HardwareTimer*)
{
}


void zAxisInterrupt(HardwareTimer*)
{
}


void aAxisInterrupt(HardwareTimer*)
{
}

void setInterrupt(int axis, float delay){
  Tim[axis]->setMode(1, TIMER_OUTPUT_COMPARE);
  Tim[axis]->setOverflow(delay, MICROSEC_FORMAT);
  switch(axis) {
        case 0:
            Tim[axis]->attachInterrupt(xAxisInterrupt);
            break;
        case 1:
            Tim[axis]->attachInterrupt(zAxisInterrupt);
            break;
        case 2:
            Tim[axis]->attachInterrupt(aAxisInterrupt);
            break;
  }
  Tim[axis]->resume();
}

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
