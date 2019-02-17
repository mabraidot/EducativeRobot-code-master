#include <Arduino.h>
#include "config.h"
#include "debug.h"

void Debug::print(String message){
    if(DEBUG){
        Serial.print(message);
    }
}

void Debug::println(String message){
    if(DEBUG){
        Serial.println(message);
    }
}

void Debug::print(byte message){
    if(DEBUG){
        Serial.print(message);
    }
}

void Debug::println(byte message){
    if(DEBUG){
        Serial.println(message);
    }
}
