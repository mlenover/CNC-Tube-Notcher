#ifndef COMMAND_SOURCE_H
#define COMMAND_SOURCE_H

#include <Arduino.h>

class ICommandSource {

  private:

  public:
    void readyForData();
    String getData();
    void printError(String error);
};
#endif
