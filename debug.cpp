#include "config.h"
#include "debug.h"

void Debug::print(String message, bool eol=false){
  if(DEBUG){
    if(eol){
      Serial.println(message);
    }else{
      Serial.print(message);
    }
  }
}
void Debug::print(String message){
  print(message, false);
}

void Debug::println(String message){
  print(message, true);
}
