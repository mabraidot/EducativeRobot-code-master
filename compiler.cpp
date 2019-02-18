#include <Arduino.h>
#include "config.h"
#include "debug.h"
#include "compiler.h"

Compiler compiler;

void Compiler::init(void){
    pinMode(RUN_BUTTON, INPUT);
    pinMode(RUN_LED, OUTPUT);
    pinMode(STEPS_BUTTON, INPUT);
    pinMode(STEPS_LED, OUTPUT);
}