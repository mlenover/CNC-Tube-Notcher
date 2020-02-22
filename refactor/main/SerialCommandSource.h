#ifndef SERIAL_COMMAND_SOURCE_H
#define SERIAL_COMMAND_SOURCE_H

#ifndef COMMAND_SOURCE_H
#include "ICommandSource.h"
#endif

#include <Arduino.h>

class SerialCommandSource: ICommandSource {

  private:
    char c;

  public:
    virtual void readyForData();
    virtual String getData();
    virtual void printError(String error);

};
#endif
