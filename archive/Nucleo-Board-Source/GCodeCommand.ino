#include <GCodeCommand.h>

GCodeCommand::GCodeCommand()
{   
}

void GCodeCommand::clear()
{
    setCode(UNKNOWN);
    for(int i=0; i < GCODE_MAX_PARAMS; i++) {
        param[i] = 0;
    }
}

gcode GCodeCommand::getCode()
{
    return code;
}

void GCodeCommand::setCode(gcode value) 
{
    code = value;
}
        
double GCodeCommand::getParameter(char paramName)
{
    int paramIndex = paramName - 65;
    if(paramIndex >= 0 && paramIndex < GCODE_MAX_PARAMS) {
         return param[paramIndex];
     }
    return 0;
}

void GCodeCommand::setParameter(char paramName, double value)
{
     int paramIndex = paramName - 65;
     if(paramIndex >= 0 && paramIndex < GCODE_MAX_PARAMS) {
         param[paramIndex] = value;
     }
}  
        
bool GCodeCommand::parse(String commandText)
{
    clear();
    int spaceIndex = commandText.indexOf(' ');
    String codeStr;
    if(spaceIndex != -1) {
        codeStr = commandText.substring(0, spaceIndex);
    }
    else {
        codeStr = commandText;
    }
    char codeLetter = codeStr.charAt(0);
    int codeNumber = codeStr.substring(1).toInt();
    switch(codeLetter) {
        case 'G':
            switch(codeNumber) {
                case 0:
                    code = G00;
                    break;
                case 4:
                    code = G04;
                    break;
                case 90:
                    code = G90;
                    break;
                case 91:
                    code = G91;
                    break;
                case 92:
                    code = G92;
                    break;
                default:
                    return false;
            }
            break;
        case 'M':
            switch(codeNumber) {
                case 18:
                    code = M18;
                    break;
                case 100:
                    code = M100;
                    break;
                case 114:
                    code = M114;
                    break;
                default:
                    return false;
            }
            break;
        default:
            return false;
    }

    while(spaceIndex != -1) {
        commandText = commandText.substring(spaceIndex + 1);
        spaceIndex = commandText.indexOf(' ');
        String paramStr;
        if(spaceIndex != -1) {
            paramStr = commandText.substring(0, spaceIndex);
        }
        else {
            paramStr = commandText;
        }
        char paramLetter = paramStr.charAt(0);
        double paramNumber = paramStr.substring(1).toFloat();
        setParameter(paramLetter, paramNumber);
    }

    return true;
}

void setup(){
    GCodeCommand *g = new GCodeCommand();

    bool ok = g->parse("G90 X1.1 Y2.2 Z3.3");
    if( ok == true) {
        gcode code = g->getCode();
        double x = g->getParameter('X');
        double z = g->getParameter('z');
    }


}

void loop(){}