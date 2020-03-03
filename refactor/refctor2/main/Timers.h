#ifndef TIMERS_H
#define TIMERS_H

#include <Arduino.h>
#include "MoveRunner.h"
#include "Param.h"

class Timers {
    private:      
        HardwareTimer* Tim1 = new HardwareTimer(TIM3);
        HardwareTimer* Tim2 = new HardwareTimer(TIM4);
        HardwareTimer* Tim3 = new HardwareTimer(TIM5);
        HardwareTimer* Tim[param.numAxes] = {Tim1, Tim2, Tim3};

    public:
        void init(void (*x_handler)(HardwareTimer *), void (*z_handler)(HardwareTimer *), void (*a_handler)(HardwareTimer *));
        void stopInterrupt(int axis);
        void stopInterrupts();
        void resumeInterrupt(int axis);
        void resumeInterrupts();
        void setInterruptDelay(int axis, float delay);
        void resetCounter(int axis);
        void detachAllInterrupts();
};

#endif
