#ifndef DEBUG_H
#define DEBUG_H

#include "Arduino.h"

class Debug {

  public:
    
    Debug() {};

    void print(String message, bool eol);
    void print(String message);
    void println(String message);


};

extern Debug debug;

#endif //DEBUG_H