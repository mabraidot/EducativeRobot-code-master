#include <Arduino.h>
#include "config.h"
#include "debug.h"
#include "blocks.h"
#include "compiler.h"


/*--------------------------------------*/
/*       PUBLIC FUNCTIONS               */
/*--------------------------------------*/
void Compiler::init(void){
    pinMode(RUN_BUTTON, INPUT_PULLUP);
    pinMode(RUN_LED, OUTPUT);
    pinMode(STEPS_BUTTON, INPUT_PULLUP);
    pinMode(STEPS_LED, OUTPUT);
}


void Compiler::run(void){
    // @TODO: read i2c and start program

    if(!_compiled){
        // One step run
        if(digitalRead(RUN_BUTTON) == LOW){
            digitalWrite(RUN_LED, HIGH);
            _scanBlocks();
            digitalWrite(RUN_LED, LOW);
        }

        // Step by step run
        if(digitalRead(STEPS_BUTTON) == LOW){
            digitalWrite(STEPS_LED, HIGH);
            _scanBlocks();
            digitalWrite(STEPS_LED, LOW);
        }
    }else{

        // Run the program
        if(!_busy){

        }
        /*
        if compiler not busy
            load next block
            if next was ok
                set compiler busy
            else
                set compiler not busy
                set _compiled false
                clean index buffers
        else
            execute block

        --Load next block index
        determine if it is a function or normal block
        if is any block left (function or normal)
            if block is function call
                set index temporarily to later continue there 
                set inside function flag
            set block index on queue
            return true
        else
            if it is function block
                clean inside function flag
                return true
            else
                return false


        --Execute block
        load index from queue
        if block exists
            if is a function block
                light on the led
                set compiler not busy
                return
            execute block
            if block has modifiers
                ***?
            if execution is over
                if was a function call
                    turn off the block function led
                set compiler not busy
            
        */

        _compiled = false;
    }

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
        
        delay(1000);
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
        
        delay(600);
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
    uint8_t _data = 14;
    byte _block_position = 1;
    byte _block_modifier_position = 1;
    
    while(_finding){

        _finding_modifier = true;
        _block_modifier_position = 1;
        while(_finding_modifier){
            _finding_modifier = false;
            if(_addSlave(_function_mode, true, _block_address, _block_position, _block_modifier_position)){
                _finding_modifier = true;
                _block_modifier_position++;
                _block_address++;
            }
        }

        if((_function_mode && blocks._functions[1].address) || (!_function_mode && blocks._blocks[1].address)){
            _block_position++;
        }
        _finding = false;
        if(_addSlave(_function_mode, false, _block_address, _block_position, 0)){
            _finding = true;
            _block_address++;
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
    blocks.off_leds();

    _compiled = true;
}
