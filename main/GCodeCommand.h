#ifndef GCodeCommand_h
#define GCodeCommand_h
#endif

#include <WString.h>

enum gcode {
    UNKNOWN,
    G00,
    G01,
    G04,
    G28, //Home
    G90, //absolute coordinates
    G91, //relative coordinates
    G92,
    M02,
    M18,
    M100,
    M114
};

#define GCODE_MAX_PARAMS 26

class GCodeCommand {
    private:
        gcode code;
        double param[GCODE_MAX_PARAMS];
        bool hasParam[GCODE_MAX_PARAMS];
        
    public:
        GCodeCommand();
        void clear();
        gcode getCode();
        void setCode(gcode value);
        void setParameter(char paramName, double value);
        double getParameter(char paramName, bool &hasParam);
        bool parse(String commandText);
};
