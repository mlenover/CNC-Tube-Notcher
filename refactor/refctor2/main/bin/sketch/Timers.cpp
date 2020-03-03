#include "Timers.h"

void Timers::init(void (*x_handler)(HardwareTimer *), void (*z_handler)(HardwareTimer *), void (*a_handler)(HardwareTimer *)){
    Tim1->attachInterrupt(x_handler);
    Tim2->attachInterrupt(z_handler);
    Tim3->attachInterrupt(a_handler);

    for(int i = 0; i < param.numAxes; i++){
        resetCounter(i);
    }

    stopInterrupts();
}

void Timers::stopInterrupt(int axis){
    Tim[axis]->pause();
}

void Timers::stopInterrupts(){
    for(int i = 0; i < param.numAxes; i++){
            Tim[i]->pause();
    }
}

void Timers::resumeInterrupt(int axis){
    Tim[axis]->resume();
}

void Timers::resumeInterrupts(){
    for(int i = 0; i < param.numAxes; i++){
            Tim[i]->resume();
    }
}

void Timers::setInterruptDelay(int axis, float delay){
    Tim[axis]->setOverflow(delay, MICROSEC_FORMAT);
}

void Timers::resetCounter(int axis){
    Tim[axis]->setCount(0,TICK_FORMAT);
}