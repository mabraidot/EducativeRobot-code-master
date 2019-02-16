#include "config.h"
#include "debug.h"

void Debug::print(String message){
  Serial.print(message);
}

void Debug::println(String message){
  Serial.println(message);
}

void Debug::print(byte message){
  Serial.print(message);
}

void Debug::println(byte message){
  Serial.println(message);
}
