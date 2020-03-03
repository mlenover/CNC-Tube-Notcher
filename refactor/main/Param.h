#ifndef PARAM_H
#define PARAM_H

#include <Arduino.h>

class Param {

  public:
    static const int numAxes = 3;

    const float stepsPerMMX = 10;
    const float stepsPerMMZ = 10;
    const float stepsPerMMA = 10;
    const float stepsPer[numAxes] = {stepsPerMMX, stepsPerMMZ, stepsPerMMA};

    const float maxXSpeed = 100;
    const float maxZSpeed = 100;
    const float maxASpeed = 100;
    const int maxSpeed[numAxes] = {round(maxXSpeed*stepsPerMMX), round(maxZSpeed*stepsPerMMZ), round(maxASpeed*stepsPerMMA)};

    const float maxXAccel = 100;
    const float maxZAccel = 100;
    const float maxAAccel = 100;
    const int maxAccel[numAxes] = {round(maxXAccel*stepsPerMMX), round(maxZAccel*stepsPerMMZ), round(maxAAccel*stepsPerMMA)};

    const float minXBound = -100;
    const float maxXBound = 100;
    const float minZBound = -100;
    const float maxZBound = 100;
    const float minABound = -100;
    const float maxABound = 100;
    const int minBound[numAxes] = {round(minXBound*stepsPerMMX), round(minZBound*stepsPerMMZ), round(minABound*stepsPerMMA)};
    const int maxBound[numAxes] = {round(maxXBound*stepsPerMMX), round(maxZBound*stepsPerMMZ), round(maxABound*stepsPerMMA)};

    const float zeroPosX = 0;
    const float zeroPosZ = 30;
    const float zeroPosA = 0;
    const int zeroPos[numAxes] = {round(zeroPosX*stepsPerMMX), round(zeroPosZ*stepsPerMMZ), round(zeroPosA*stepsPerMMA)};

    const char axes[numAxes] = {'X','Z','A'};

    const float minFeedrate = 100;
    const float maxTurnDelta = 10;
    const float maxCentripalAccel = 5;

    const int parseTime = 1000; //ms of movements to parse before executing

    static const int maxAccelSteps = 1000;

    const int defaultXPos = 0;
    const int defaultZPos = 10;
    const int defaultAPos = 0;
    
    static const int numMoveBuf = 10;

    static const int precision = 1000000; //Microseconds
};

static Param param;
#endif
