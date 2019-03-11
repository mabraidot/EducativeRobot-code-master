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


// Blocking sound, using delay
void Buzzer::error(void){
    
    startSound();

    tone(BUZZER, 1250, 200);
    delay(250);
    tone(BUZZER, 1250, 200);
    delay(250);
    tone(BUZZER, 250, 500);
    delay(500);
    noTone(BUZZER);

}


// Blocking sound, using delay
void Buzzer::startUp(void){
    
    startSound();
    
    for(byte i = 0; i < 20; i+=2){
        //_note(i, 1500*sqrt((double)i), 200000/sqrt((double)i));
        //_note(i+1, 0, 50000/sqrt((double)i));
        tone(BUZZER, 1500*sqrt((double)i), 200/sqrt((double)i));
        delay(250/sqrt((double)i));
        noTone(BUZZER);
    }
    

}


// Blocking sound, using delay
void Buzzer::compilationOk(void){
    
    startSound();

    tone(BUZZER, 2900, 50);
    delay(100);
    tone(BUZZER, 2900, 50);
    delay(100);
    noTone(BUZZER);

}


// Blocking sound, using delay
void Buzzer::blockExecutionBegining(void){
    
    startSound();

    tone(BUZZER, 1900, 50);
    delay(100);
    noTone(BUZZER);

}


void Buzzer::blockExecutionRunning(void){

    static unsigned long pause_interval = micros();
    if(micros() > (pause_interval + 200000)){
        pause_interval = micros();
        startSound();
    }
    _note(0, 400, 80000);
    
}


// Blocking sound, using delay
void Buzzer::executionEnd(void){
    
    startSound();

    tone(BUZZER, 4000, 100);
    delay(140);
    tone(BUZZER, 1900, 50);
    delay(90);
    tone(BUZZER, 1900, 50);
    delay(90);
    tone(BUZZER, 4000, 80);
    delay(120);

    noTone(BUZZER);

}
