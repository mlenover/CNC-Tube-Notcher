#ifndef GCODE_COMMAND_H
#include "GCodeCommand.h"


void GCodeCommand::setup()
{   
}

void GCodeCommand::clear()
{
    setCode(UNKNOWN);
    for(int i=0; i < GCODE_MAX_PARAMS; i++) {
        param[i] = 0;
        hasParam[i] = false;
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
        
double GCodeCommand::getParameter(char paramName, bool &hasValue)
{
    int paramIndex = paramName - 65;
    if(paramIndex >= 0 && paramIndex < GCODE_MAX_PARAMS) {
         hasValue = hasParam[paramIndex];
         return param[paramIndex];
    }
    hasValue = false;
    return 0;
}

void GCodeCommand::setParameter(char paramName, double value)
{
     int paramIndex = paramName - 65;
     if(paramIndex >= 0 && paramIndex < GCODE_MAX_PARAMS) {
         param[paramIndex] = value;
         hasParam[paramIndex] = true;
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
                case 1:
                    code = G01;
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
                case 3:
                    code = M03;
                    break;
                case 5:
                    code = M05;
                    break;
                case 7:
                    code = M07;
                    break;
                case 9:
                    code = M09;
                    break;
                case 10:
                    code = M10;
                    break;
                case 11:
                    code = M11;
                    break;
                case 14:
                    code = M14;
                    break;
                case 15:
                    code = M15;
                    break;
                case 98:
                    code = M98;
                    break;
                case 99:
                    code = M99;
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

#endif
