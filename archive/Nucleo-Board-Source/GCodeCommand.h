#ifndef GCodeCommand_h
#define GCodeCommand_h

enum gcode {
    UNKNOWN,
    G00,
    G04,
    G90,
    G91,
    G92,
    M18,
    M100,
    M114
};

#define GCODE_MAX_PARAMS 26

class GCodeCommand {
    private:
        gcode code;
        double param[GCODE_MAX_PARAMS];
        
    public:
        GCodeCommand();
        void clear();
        gcode getCode();
        void setCode(gcode value);
        void setParameter(char paramName, double value);
        double getParameter(char paramName);
        bool parse(String commandText);
};

#endif