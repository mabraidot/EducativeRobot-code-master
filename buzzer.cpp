#include <Arduino.h>
#include "config.h"
#include "buzzer.h"


void Buzzer::init(void){
    pinMode(BUZZER, OUTPUT);
}


void Buzzer::_note(byte index, uint16_t frequency, uint32_t duration){
    
    if(index == _melody_index){

        if(frequency) tone(BUZZER, frequency);
        
        if(micros() > (_previous_time + duration)){
            if(frequency) noTone(BUZZER);
            _previous_time = micros();
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
    _previous_time = micros();
}


void Buzzer::error(void){
    
    byte i = 0;
    _note(i, 1243, 200000);    i++;
    _note(i, 0, 50000);        i++; // White noise
    _note(i, 1243, 200000);    i++;
    _note(i, 0, 50000);        i++; // White noise
    _note(i, 250, 500000);     i++;
}


void Buzzer::startUp(void){
    /*tone(BUZZER, 1500, 100);
    delay(150);
    
    tone(BUZZER, 3200, 100);
    delay(150);
    
    tone(BUZZER, 2000, 100);
    delay(150);

    noTone(BUZZER);*/
    for(byte i = 0; i < 20; i+=2){
        _note(i, 1500*sqrt((double)i), 200000/sqrt((double)i));
        _note(i+1, 0, 50000/sqrt((double)i));
        //tone(BUZZER, 1500*sqrt((double)i), 200000/sqrt((double)i));
        //delay(50000/sqrt((double)i));
    }
    

}


void Buzzer::compilationOk(void){
    
    byte i = 0;
    _note(i, 2900, 50000);    i++;
    _note(i, 0, 50000);       i++; // White noise
    _note(i, 2900, 50000);    i++;
    _note(i, 0, 50000);       i++; // White noise

}


void Buzzer::blockExecutionBegining(void){
    
    byte i = 0;
    _note(i, 1900, 50000);    i++;
    
}


void Buzzer::blockExecutionRunning(void){

    //byte i = 0;
    for(byte i = 0; i < 20; i+=2){
        _note(i, 400, 80000);
        _note(i+1, 0, 500000);
    }
}


void Buzzer::executionEnd(void){
    
    byte i = 0;
    _note(i, 4000, 100000);   i++;
    _note(i, 0, 40000);       i++; // White noise
    _note(i, 1900, 50000);    i++;
    _note(i, 0, 40000);       i++; // White noise
    _note(i, 1900, 50000);    i++;
    _note(i, 0, 40000);       i++; // White noise
    _note(i, 4000, 80000);    i++;
    _note(i, 0, 40000);       i++; // White noise
    
}

