#include <Arduino.h>
#include <Wire.h>
extern "C" {
  #include "utility/twi.h"
}
#include "config.h"
#include "debug.h"
#include "blocks.h"
#include "compiler.h"


/*--------------------------------------*/
/*       PUBLIC FUNCTIONS               */
/*--------------------------------------*/
void Compiler::init(void){
    pinMode(RUN_BUTTON, INPUT);
    pinMode(RUN_LED, OUTPUT);
    pinMode(STEPS_BUTTON, INPUT);
    pinMode(STEPS_LED, OUTPUT);
}


void Compiler::run(void){
    // @TODO: read i2c and start program
    digitalWrite(RUN_LED, HIGH);
    _scanBlocks();
}


void Compiler::runSteps(void){
    // @TODO: read i2c and start step by step program
    digitalWrite(STEPS_LED, HIGH);
    _scanBlocks();
}


boolean Compiler::readRunButton(void){
    return _readButton(RUN_BUTTON);
}


boolean Compiler::readStepsButton(void){
    return _readButton(STEPS_BUTTON);
}




/*--------------------------------------*/
/*      PRIVATE FUNCTIONS               */
/*--------------------------------------*/
boolean Compiler::_addSlave(boolean _function_mode, boolean _modifier_mode, byte _block_address, byte _block_position, byte _block_modifier_position){

    debug.print(_block_address);

    if(_modifier_mode){
        debug.print((_function_mode) ? F("\tFUNCTION: Adding slave modifier ...\n") : F("\tBLOCKS: Adding slave modifier ...\n"));
        debug.print(F("\tAdding slave modifier ...\n"));
        blocks.add_slave(SLAVE_MODIFIER_ADDRESS, _block_address);
        delay(800);
        if(blocks.read_state(_block_address, STATE_ACTIVATED)){
            
            if(_function_mode){
                blocks._functions[_block_position].modifiers[_block_modifier_position].address = _block_address;
                blocks._functions[_block_position].modifiers[_block_modifier_position].type = blocks.read_state(_block_address, STATE_FUNCTION);
                blocks._functions[_block_position].modifiers[_block_modifier_position].value = blocks.read_state(_block_address, STATE_VALUE);
                blocks.flash_led(blocks._functions[_block_position].modifiers[_block_modifier_position].address,STATE_LED_ON);
            }else{
                blocks._blocks[_block_position].modifiers[_block_modifier_position].address = _block_address;
                blocks._blocks[_block_position].modifiers[_block_modifier_position].type = blocks.read_state(_block_address, STATE_FUNCTION);
                blocks._blocks[_block_position].modifiers[_block_modifier_position].value = blocks.read_state(_block_address, STATE_VALUE);
                blocks.flash_led(blocks._blocks[_block_position].modifiers[_block_modifier_position].address,STATE_LED_ON);
            }
            blocks.open_gate(_block_address);
            delay(300);
                
            return true;
        }else{
            return false;
        }
    }else{
        debug.print((_function_mode) ? F("\tFUNCTION: Adding slave ...\n") : F("\tBLOCKS: Adding slave ...\n"));
        blocks.add_slave(SLAVE_ADDRESS, _block_address);
        delay(800);
        if(blocks.read_state(_block_address, STATE_ACTIVATED)){
            
            if(_function_mode){
                blocks._functions[_block_position].address = _block_address;
                blocks._functions[_block_position].type = blocks.read_state(_block_address, STATE_FUNCTION);
                blocks._functions[_block_position].value = blocks.read_state(_block_address, STATE_VALUE);
                blocks.flash_led(blocks._functions[_block_position].address,STATE_LED_ON);
            }else{
                blocks._blocks[_block_position].address = _block_address;
                blocks._blocks[_block_position].type = blocks.read_state(_block_address, STATE_FUNCTION);
                blocks._blocks[_block_position].value = blocks.read_state(_block_address, STATE_VALUE);
                blocks.flash_led(blocks._blocks[_block_position].address,STATE_LED_ON);
            }
            blocks.open_gate(_block_address);
            delay(300);
                
            return true;
        }else{
            return false;
        }
    }

}


byte Compiler::_scanI2CBuffer(byte _block_address, boolean _function_mode){
    
    bool _finding = true;
    bool _finding_modifier = true;
    uint8_t _rc = 1;
    uint8_t _data = 0;
    byte _block_position = 0;
    byte _block_modifier_position = 0;
    
    while(_finding){

        _finding_modifier = true;
        _block_modifier_position = 0;
        while(_finding_modifier){
            _finding_modifier = false;
            _rc = twi_writeTo(SLAVE_MODIFIER_ADDRESS, &_data, 0, 1, 0);
            if(_rc == 0){
                if(_addSlave(_function_mode, true, _block_address, _block_position, _block_modifier_position)){
                    _finding_modifier = true;
                    _block_modifier_position++;
                    _block_address++;
                }
            }
        }

        if((_function_mode && blocks._functions[0].address) || (!_function_mode && blocks._blocks[0].address)){
            _block_position++;
        }
        _finding = false;
        _rc = twi_writeTo(SLAVE_ADDRESS, &_data, 0, 1, 0);
        if(_rc == 0){
            if(_addSlave(_function_mode, false, _block_address, _block_position, 0)){
                _finding = true;
                _block_address++;
            }
        }
    }

    return _block_address;
}


void Compiler::_scanBlocks(void){
    
    blocks.disable_function();
    blocks.disable_slaves();
    blocks.empty_blocks();

    byte _block_address = SLAVE_START_ADDRESS;
    // Scan function
    blocks.enable_function();
    _block_address = _scanI2CBuffer(_block_address, true);
    // Scan slaves
    blocks.enable_slaves();
    _block_address = _scanI2CBuffer(_block_address, false);

    blocks.scanResults();
    delay(1000);
    blocks.off_leds();
}


boolean Compiler::_readButton(uint8_t button_pin){

    boolean newPushButton = false;

    /* Step 1: Update the integrator based on the input signal.  Note that the
    integrator follows the input, decreasing or increasing towards the limits as
    determined by the input state (0 or 1). */
    _buttonInput = digitalRead(button_pin);
    if (_buttonInput == false){
        if (_buttonIntegrator > 0){
            _buttonIntegrator--;
        }
    }else if (_buttonIntegrator < DEBOUNCE_MAXIMUM){
        _buttonIntegrator++;
    }
    /* Step 2: Update the output state based on the integrator.  Note that the
    output will only change states if the integrator has reached a limit, either
    0 or DEBOUNCE_MAXIMUM. */
    if (_buttonIntegrator == 0){
        _buttonOutput = false;
    }else if (_buttonIntegrator >= DEBOUNCE_MAXIMUM){
        if(_buttonInput != _buttonOutput){
            newPushButton = true;
        }
        _buttonOutput = true;
        _buttonIntegrator = DEBOUNCE_MAXIMUM;  /* defensive code if integrator got corrupted */
    }
    
    return newPushButton;
}