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

    // Step by step run
    if(digitalRead(STEPS_BUTTON) == LOW){
        digitalWrite(STEPS_LED, HIGH);
        _scanBlocks();
        digitalWrite(STEPS_LED, LOW);
    }

    if(!_compiled){
        // One step run
        if(digitalRead(RUN_BUTTON) == LOW){
            digitalWrite(RUN_LED, HIGH);
            _scanBlocks();
            digitalWrite(RUN_LED, LOW);
        }

        
    }else{

        // Run the program
        if(!_busy){
            if(_next()){
                
                // DEMO
                blink_timeout = millis() + blink_interval;

            }else{
                _busy = false;
                _compiled = false;
                _queue = 0;
                _queue_temp = 0;
            }
        }else{
            _execute();
        }
    }

}




/*--------------------------------------*/
/*      PRIVATE FUNCTIONS               */
/*--------------------------------------*/
boolean Compiler::_next(void){
    
    _queue++;
    
    if((_function_flag && blocks._functions[_queue].address) 
        || (!_function_flag && blocks._blocks[_queue].address)){

            if(!_function_flag && blocks._blocks[_queue].type == MODE_FUNCTION){
                _queue_temp = _queue;
                _queue = 0;
                _function_flag = true;
                _busy = false;
            }else{
                _busy = true;
            }
            return true;
    }else{
        if(_function_flag){
            _function_flag = false;
            _queue = _queue_temp;
            _busy = true;
            return true;
        }else{
            _queue = 0;
            _queue_temp = 0;
            return false;
        }
    }
}


void Compiler::_execute(void){

    byte current_address = 0;
    if(_function_flag){
        current_address = blocks._functions[_queue].address;
    }else{
        current_address = blocks._blocks[_queue].address;
    }
    if(!current_address){
        return;
    }
    

    
    /***** DEMO *******/
    if(blocks.read_state(current_address, STATE_LED) != STATE_LED_BLINK){
        blocks.flash_led(current_address, STATE_LED_BLINK);
    }
    if(blink_timeout < millis()){
        debug.print(F("Just executed: "));
        debug.println(current_address);
    
        blocks.flash_led(current_address, STATE_LED_OFF);
        blink_timeout = millis() + blink_interval;

        _busy = false;
    }
    /***** END DEMO *******/
        
    
    if(_queue_temp && _function_flag && blocks._blocks[_queue_temp].type == MODE_FUNCTION){
        blocks.flash_led(blocks._blocks[_queue_temp].address, STATE_LED_ON);
    }
    if(_queue_temp && !_function_flag){
        blocks.flash_led(blocks._blocks[_queue_temp].address, STATE_LED_OFF);
        //_busy = false;
    }
    
}


boolean Compiler::_addSlave(boolean _function_mode, boolean _modifier_mode, byte _block_address, byte _block_position, byte _block_modifier_position){

    debug.print(_block_address);

    if(_modifier_mode){
        debug.print((_function_mode) ? F("\tFUNCTION: Adding slave modifier ...\n") : F("\tBLOCKS: Adding slave modifier ...\n"));
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
