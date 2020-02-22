#ifndef GCODE_COMMAND_H
#define GCODE_COMMAND_H

#include <Arduino.h>

enum gcode {
    UNKNOWN,
    G00,
    G01,
    G04,
    G28, //Home
    G90, //absolute coordinates
    G91, //relative coordinates
    G92,
    M03,
    M05,
    M07,
    M09,
    M10,
    M11,
    M14,
    M15,
    M98,
    M99
};


class GCodeCommand {
    private:
        static const int GCODE_MAX_PARAMS = 26;
        gcode code;
        double param[GCODE_MAX_PARAMS];
        bool hasParam[GCODE_MAX_PARAMS];
        
    public:
        GCodeCommand* prevCmd;
        GCodeCommand* nextCmd;
        void setup();
        void clear();
        gcode getCode();
        void setCode(gcode value);
        void setParameter(char paramName, double value);
        double getParameter(char paramName, bool &hasParam);
        bool parse(String commandText);
        bool absMode = true;
};

#endif
