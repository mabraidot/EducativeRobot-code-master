#include <Arduino.h>
#include "config.h"
#include "debug.h"
#include "blocks.h"
#include "compiler.h"
#include "rf.h"
#include "buzzer.h"


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
    
    // Step by step run
    if(digitalRead(STEPS_BUTTON) == LOW){
        if(!_compiled){
            _queue = 0;
            _queue_temp = 0;
            _steps_flag = true;
            _led(STEPS_LED, STATE_LED_ON);
            scanBlocks();
            _led(STEPS_LED, STATE_LED_OFF);
        }else{
            _steps_busy = true;
            _led(STEPS_LED, STATE_LED_ON);
        }
    }

    if(_compiled && _steps_flag && !_steps_busy){
        _led(STEPS_LED, STATE_LED_BLINK);
    }else if(!_compiled){
        _led(RUN_LED, STATE_LED_OFF);
        _led(STEPS_LED, STATE_LED_OFF);
    }

    if(!_compiled){
        // One step run
        if(digitalRead(RUN_BUTTON) == LOW){
            _queue = 0;
            _queue_temp = 0;
            _steps_flag = false;
            _steps_busy = false;
            _led(RUN_LED, STATE_LED_ON);
            scanBlocks();
            delay(600);
        }

        
    }else{

        // Run the program
        if(!_busy){
            if(!_next()){
                _busy = false;
                _queue = 0;
                _queue_temp = 0;
                _steps_flag = false;
                //_compiled = false;
                //_steps_busy = false;
                blocks.disable_function();
                blocks.disable_slaves();
                //buzzer.executionEnd();

                if(!rf.sent){
                    if(!rf.sendMessage(MODE_END_OF_PROGRAM, 0, true)){
                        _steps_busy = false;
                        _compiled = false;
                        buzzer.executionEnd();
                    }
                }else{
                    if(rf.receiveMessageTimeout(1000)){
                        _compiled = false;
                        buzzer.executionEnd();
                        char *response = rf.getBuffer();
                        debug.print(F("Execution response: "));
                        debug.println((char *)response);
                        rf.sent = false;
                        _steps_busy = false;
                    }
                }
                
            }
        }else{
            if(!_steps_flag || _steps_busy){
                _execute();
            }else if(_steps_flag && !_steps_busy){
                _execute_paused();
            }
        }
    }

}




/*--------------------------------------*/
/*      PRIVATE FUNCTIONS               */
/*--------------------------------------*/
byte Compiler::_get_modifiers_count(boolean _function_flag, byte queue){

    byte _modifiers_count = 0;
    if(_function_flag){
        for( byte j = 1; j <= FUNCTION_MODIFIERS_COUNT; j++ ){
            if(blocks._functions[queue].modifiers[j].address){
                _modifiers_count++;
            }
        }
    }else{
        for( byte j = 1; j <= SLAVES_MODIFIERS_COUNT; j++ ){
            if(blocks._blocks[queue].modifiers[j].address){
                _modifiers_count++;
            }
        }
    }

    return _modifiers_count;

}


void Compiler::_set_modifier_values(boolean _function_flag, byte queue, byte i, byte *_type, byte *_loop, byte *_old_value, byte *_address){

    if(_function_flag){
        *_type = blocks._functions[queue].modifiers[i].type;
        *_loop = blocks._functions[queue].modifiers[i].loop_value;
        *_old_value = blocks._functions[queue].modifiers[i].value;
        *_address = blocks._functions[queue].modifiers[i].address;
    }else{
        *_type = blocks._blocks[queue].modifiers[i].type;
        *_loop = blocks._blocks[queue].modifiers[i].loop_value;
        *_old_value = blocks._blocks[queue].modifiers[i].value;
        *_address = blocks._blocks[queue].modifiers[i].address;
    }

}


boolean Compiler::_next(void){
    
    _queue++;
    byte _type, _loop, _old_value, _address = 0;

    // Modifiers
    byte _modifiers_count = _get_modifiers_count(_function_flag, _queue-1);
    if(_modifiers_count > 0){
        for( byte i = 1; i <= _modifiers_count; i++ ){
            _type, _loop, _old_value, _address = 0;
            _set_modifier_values(_function_flag, _queue-1, i, &_type, &_loop, &_old_value, &_address);
            if(_type == MODE_MODIFIER_LOOP){
                if(_loop > 1){

                    _queue--;
                    
                    if(_function_flag){
                        blocks._functions[_queue].modifiers[i].loop_value = _loop-1;
                    }else{
                        blocks._blocks[_queue].modifiers[i].loop_value = _loop-1;
                    }

                }else{
                    // Reset loop_value and let leds turned on
                    blocks.flash_led(_address, STATE_LED_ON);
                    delay(100);
                    blocks.flash_led(_address, STATE_LED_OFF);
                    delay(50);
                    
                    if(_function_flag){
                        blocks._functions[_queue-1].modifiers[i].loop_value = _old_value;
                        blocks.set_state(_address, STATE_VALUE, _old_value);
                    }else{
                        blocks._blocks[_queue-1].modifiers[i].loop_value = _old_value;
                        blocks.set_state(_address, STATE_VALUE, _old_value);
                    }
                    delay(50);
                }
            }else{
                // Do other stuff
            }
        }
    }
    // End modifiers

    // If it's first loop modifier, flash its led
    _modifiers_count = _get_modifiers_count(_function_flag, _queue);
    if(_modifiers_count > 0){
        for( byte i = 1; i <= _modifiers_count; i++ ){
            _type, _loop, _old_value, _address = 0;
            _set_modifier_values(_function_flag, _queue, i, &_type, &_loop, &_old_value, &_address);
            if(_type == MODE_MODIFIER_LOOP){
                if(_loop > 0){

                    // Little flash effect on loop number
                    if(blocks.read_state(_address, STATE_VALUE) != _loop-1){
                        blocks.flash_led(_address, STATE_LED_ON);
                        delay(100);
                        blocks.flash_led(_address, STATE_LED_OFF);
                        delay(50);
                    }
                    // Set loop number on display
                    blocks.set_state(_address, STATE_VALUE, _loop-1);
                    delay(50);
                    
                }else{
                    _busy = false;
                    _steps_busy = false;
                    return true;
                }
            }
        }
    }



    // Previous block has finished, let its led on
    if(_function_flag && blocks._functions[_queue-1].address){
        blocks.flash_led(blocks._functions[_queue-1].address, STATE_LED_ON);
        delay(50);
    }else if(blocks._blocks[_queue-1].address){
        blocks.flash_led(blocks._blocks[_queue-1].address, STATE_LED_ON);
        delay(50);
    }

    
    if((_function_flag && blocks._functions[_queue].address) 
        || (!_function_flag && blocks._blocks[_queue].address)){
            
            if(!_function_flag && blocks._blocks[_queue].type == MODE_FUNCTION){
                _queue_temp = _queue;
                _queue = 0;
                _function_flag = true;
                blocks.off_leds(true);  // Turn off function leds
                
                delay(50);
                // At the end of execution, leave function call block's led on
                blocks.flash_led(blocks._blocks[_queue_temp].address, STATE_LED_ON);
                delay(50);
            
            }
            _busy = true;
            
            return true;
    }else{
        if(_function_flag){
            _function_flag = false;
            _queue = _queue_temp;
            _busy = false;

            return true;
        }else{
            return false;
        }
    }
}


void Compiler::_execute_paused(void){
    
    byte current_address, current_type = 0;
    if(_function_flag){
        current_address = blocks._functions[_queue].address;
        current_type = blocks._functions[_queue].type;
    }else{
        current_address = blocks._blocks[_queue].address;
        current_type = blocks._blocks[_queue].type;
    }
    if(blocks.read_state(current_address, STATE_LED) != STATE_LED_OFF){
        blocks.flash_led(current_address, STATE_LED_OFF);
        buzzer.stopSound();
    }

}


void Compiler::_execute(void){

    byte current_address, current_type, current_value = 0;
    if(_function_flag){
        current_address = blocks._functions[_queue].address;
        current_type = blocks._functions[_queue].type;
        current_value = blocks._functions[_queue].value;
    }else{
        current_address = blocks._blocks[_queue].address;
        current_type = blocks._blocks[_queue].type;
        current_value = blocks._blocks[_queue].value;
    }
    
    // If function execution is over, return to the next step
    if(!current_address || (_queue_temp == _queue && !_function_flag)){
        _busy = false;
        _steps_busy = false;
        return;
    }
    
    // Start RF transmission, if not started yet
    if(!rf.sent){
        if(!rf.sendMessage(current_type, current_value, true)){
            // Action timed out so rise an error. At the moment, cancel transmission
            _busy = false;
            _steps_busy = false;
            _compiled = false;
            blocks.disable_function();
            blocks.disable_slaves();
            buzzer.error();
        }
        _rf_waiting_timeout = millis();
    }else{
        // Flash the led while the block is being executed
        if(blocks.read_state(current_address, STATE_LED) != STATE_LED_BLINK){
            blocks.flash_led(current_address, STATE_LED_BLINK);
            buzzer.blockExecutionBegining();
        }
        buzzer.blockExecutionRunning();
        if(rf.receiveMessage()){
            char *response = rf.getBuffer();
            debug.print(F("Execution response: "));
            debug.println((char *)response);
            if(strcmp(response, "END") == 0){
                // Robot car action finished
                rf.sent = false;
                _busy = false;
                _steps_busy = false;
            }
        }else if(millis() > (
            _rf_waiting_timeout + (
                (current_type == MODE_SLAVE_WAIT_LIGHT || 
                 current_type == MODE_SLAVE_WAIT_SOUND || 
                 current_type == MODE_SLAVE_SOUND) ? RF_WAIT_LONG_TIMEOUT : RF_WAIT_TIMEOUT
                ) 
            )){
            _rf_waiting_timeout = millis();
            // Action timed out so rise an error.
            rf.sent = false;
            _busy = false;
            _steps_busy = false;
            _compiled = false;
            blocks.disable_function();
            blocks.disable_slaves();
            buzzer.error();
        }

    }
    
}


boolean Compiler::_addSlave(boolean _function_mode, boolean _modifier_mode, byte _block_address, byte _block_position, byte _block_modifier_position){

    debug.print(_block_address);

    if(_modifier_mode){
        debug.print((_function_mode) ? F("\tFUNCTION: Adding slave modifier ...\n") : F("\tBLOCKS: Adding slave modifier ...\n"));
        blocks.add_slave(SLAVE_MODIFIER_ADDRESS, _block_address);

        delay(150);
        if(blocks.read_state(_block_address, STATE_ACTIVATED) == 1){
            
            if(_function_mode){
                blocks._functions[_block_position].modifiers[_block_modifier_position].address = _block_address;
                blocks._functions[_block_position].modifiers[_block_modifier_position].type = blocks.read_state(_block_address, STATE_FUNCTION);
                blocks._functions[_block_position].modifiers[_block_modifier_position].value = blocks.read_state(_block_address, STATE_VALUE);
                blocks._functions[_block_position].modifiers[_block_modifier_position].loop_value = blocks._functions[_block_position].modifiers[_block_modifier_position].value;
            }else{
                blocks._blocks[_block_position].modifiers[_block_modifier_position].address = _block_address;
                blocks._blocks[_block_position].modifiers[_block_modifier_position].type = blocks.read_state(_block_address, STATE_FUNCTION);
                blocks._blocks[_block_position].modifiers[_block_modifier_position].value = blocks.read_state(_block_address, STATE_VALUE);
                blocks._blocks[_block_position].modifiers[_block_modifier_position].loop_value = blocks._blocks[_block_position].modifiers[_block_modifier_position].value;
            }
            blocks.flash_led(_block_address,STATE_LED_ON);
            delay(50);
            blocks.open_gate(_block_address);
            delay(100);
                
            return true;
        }else{
            debug.println(F("... Not responding!"));
            return false;
        }
    }else{
        debug.print((_function_mode) ? F("\tFUNCTION: Adding slave ...\n") : F("\tBLOCKS: Adding slave ...\n"));
        blocks.add_slave(SLAVE_ADDRESS, _block_address);

        delay(150);
        if(blocks.read_state(_block_address, STATE_ACTIVATED) == 1){
            
            if(_function_mode){
                blocks._functions[_block_position].address = _block_address;
                blocks._functions[_block_position].type = blocks.read_state(_block_address, STATE_FUNCTION);
                blocks._functions[_block_position].value = blocks.read_state(_block_address, STATE_VALUE);
                
            }else{
                blocks._blocks[_block_position].address = _block_address;
                blocks._blocks[_block_position].type = blocks.read_state(_block_address, STATE_FUNCTION);
                blocks._blocks[_block_position].value = blocks.read_state(_block_address, STATE_VALUE);
            }
            blocks.flash_led(_block_address,STATE_LED_ON);
            delay(50);
            blocks.open_gate(_block_address);
            delay(100);
                
            return true;
        }else{
            debug.println(F("... Not responding!"));
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


void Compiler::scanBlocks(void){
    
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
    blocks.off_leds(false);

    byte non_closing_while = blocks.checkWhileStructure();
    if(!non_closing_while){
        _compiled = true;
        buzzer.compilationOk();
    }else{
        buzzer.error();
        blocks.flash_led(non_closing_while,STATE_LED_BLINK);
        delay(3000);
        blocks.flash_led(non_closing_while,STATE_LED_OFF);
        delay(50);

        blocks.disable_function();
        blocks.disable_slaves();
        blocks.empty_blocks();

    }
}


void Compiler::_led(byte pin, byte mode){
  
    // Blink
    if(mode == STATE_LED_BLINK){
        static byte led_on = 1;
        static int _blink_interval = 500;
        static unsigned long _blink_timeout = millis() + _blink_interval;

        if(_blink_timeout < millis()){
            led_on = !led_on;
            _blink_timeout = millis() + _blink_interval;
        }
        digitalWrite(pin, led_on);
    // On
    }else if(mode == STATE_LED_ON){
        digitalWrite(pin, 1);
    // Off
    }else{
        digitalWrite(pin, 0);
    }

}