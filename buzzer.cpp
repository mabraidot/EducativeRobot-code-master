#include <Arduino.h>
#include "config.h"
#include "buzzer.h"


void Buzzer::init(void){
    pinMode(BUZZER, OUTPUT);
}


void Buzzer::_note(byte index, uint16_t frequency, uint16_t duration){
    
    if(index == _melody_index){

        if(frequency) tone(BUZZER, frequency);
        
        if(millis() > (_previous_time + duration)){
            if(frequency) noTone(BUZZER);
            _previous_time = millis();
            _melody_index++;
        }

    }
}


void Buzzer::stopSound(void){
    _previous_time = 0;
    noTone(BUZZER);
}


void Buzzer::startSound(void){
    _melody_index = 0;
    _previous_time = millis();
}


void Buzzer::error(void){
    byte i = 0;
    _note(i, 3500, 400);     i++;
    _note(i, 0, 100);        i++; // White noise
    _note(i, 1243, 400);     i++;
    _note(i, 0, 100);        i++; // White noise
    _note(i, 3500, 400);     i++;
    _note(i, 0, 100);        i++; // White noise
    _note(i, 1243, 400);     i++;
    _note(i, 0, 100);        i++; // White noise
    _note(i, 3500, 400);     i++;
    _note(i, 0, 100);        i++; // White noise
    _note(i, 1243, 400);     i++;
    _note(i, 0, 100);        i++; // White noise
    
}


void Buzzer::startUp(void){
    tone(BUZZER, 1500, 100);
    delay(150);
    //noTone(BUZZER);

    tone(BUZZER, 3200, 100);
    delay(150);
    //noTone(BUZZER);

    tone(BUZZER, 2000, 100);
    delay(150);

    noTone(BUZZER);
}


void Buzzer::compilationOk(void){

}


void Buzzer::blockExecutionBegining(void){

}


void Buzzer::blockExecutionRunning(void){

}


void Buzzer::executionEnd(void){

}

