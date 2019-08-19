#include <Arduino.h>
#include <Wire.h>
#include "config.h"
#include "debug.h"
#include "blocks.h"


void Blocks::init(void){

    Wire.begin();

    pinMode(SLAVE_ACTIVATE_PIN, OUTPUT);
    digitalWrite(SLAVE_ACTIVATE_PIN, LOW);

    pinMode(FUNCTION_ACTIVATE_PIN, OUTPUT);
    digitalWrite(FUNCTION_ACTIVATE_PIN, LOW);

}


void Blocks::empty_blocks(void){
    for( byte i = 1; i <= FUNCTION_COUNT; i++ ){
        for( byte j = 1; j <= FUNCTION_MODIFIERS_COUNT; j++ ){
            _functions[i].modifiers[j] = {};
        }
        _functions[i] = {};
    }
    for( byte i = 1; i <= SLAVES_COUNT; i++ ){
        for( byte j = 1; j <= SLAVES_MODIFIERS_COUNT; j++ ){
            _blocks[i].modifiers[j] = {};
        }
        _blocks[i] = {};
    }
    for( byte i = 1; i <= WHILE_LOOPS_COUNT; i++ ){
        _while[i] = {};
    }
}


void Blocks::off_leds(boolean function_only){
    for( byte i = 1; i <= FUNCTION_COUNT; i++ ){
        if(_functions[i].address){
            flash_led(_functions[i].address,STATE_LED_OFF);
            for( byte j = 1; j <= FUNCTION_MODIFIERS_COUNT; j++ ){
                if(_functions[i].modifiers[j].address){
                    flash_led(_functions[i].modifiers[j].address,STATE_LED_OFF);
                }
            }
        }
    }
    if(!function_only){
        for( byte i = 1; i <= SLAVES_COUNT; i++ ){
            if(_blocks[i].address){
                flash_led(_blocks[i].address,STATE_LED_OFF);
                for( byte j = 1; j <= SLAVES_MODIFIERS_COUNT; j++ ){
                    if(_blocks[i].modifiers[j].address){
                        flash_led(_blocks[i].modifiers[j].address,STATE_LED_OFF);
                    }
                }
            }
        }
    }
}


void Blocks::scanResults(void){
    debug.println(F("\nScanning I2C bus..."));
    debug.println(F("FUNCTION"));
    for( byte i = 1; i <= FUNCTION_COUNT; i++ ){
        if(_functions[i].address){
            debug.print(i);
            debug.print(F(": "));
            debug.println(_functions[i].address);
            for( byte j = 1; j <= FUNCTION_MODIFIERS_COUNT; j++ ){
                if(_functions[i].modifiers[j].address){
                    debug.print(F("\t\t"));
                    debug.print(j);
                    debug.print(F(": "));
                    debug.println(_functions[i].modifiers[j].address);
                }
            }
        }
    }
    debug.println(F("SLAVES"));
    for( byte i = 1; i <= SLAVES_COUNT; i++ ){
        if(_blocks[i].address){
            debug.print(i);
            debug.print(F(": "));
            debug.println(_blocks[i].address);
            for( byte j = 1; j <= SLAVES_MODIFIERS_COUNT; j++ ){
                if(_blocks[i].modifiers[j].address){
                    debug.print(F("\t\t"));
                    debug.print(j);
                    debug.print(F(": "));
                    debug.println(_blocks[i].modifiers[j].address);
                }
            }
        }
    }
    debug.println(F("WHILE"));
    for( byte i = 1; i <= WHILE_LOOPS_COUNT; i++ ){
        if(_while[i].start){
            debug.print(i);
            debug.print(F(": "));
            debug.print(_while[i].start);
            debug.print(F(" - "));
            debug.println(_while[i].end);
        }
    }
    debug.println(F("\nScanning finished\n"));
}


byte Blocks::checkWhileStructure(void){
    
    int while_array_index = 1;

    byte address[WHILE_LOOPS_COUNT*2] = {0};
    byte address_index = 0;
    for( byte i = 1; i <= FUNCTION_COUNT; i++ ){
        if(_functions[i].address){
            if(_functions[i].type == MODE_WHILE_START){
                address[address_index] = _functions[i].address;
                address_index++;

                while(_while[while_array_index].start > 0 && while_array_index < (WHILE_LOOPS_COUNT+1)){
                    while_array_index++;
                }
                _while[while_array_index].start = _functions[i].address;
                //debug.print(while_array_index);
                //debug.print(" - ");
                //debug.println(_while[while_array_index].start);
                
            }
            if(_functions[i].type == MODE_WHILE_END){
                if(address_index > 0){
                    address_index--;
                    address[address_index] = 0;

                    while(_while[while_array_index].end > 0 && while_array_index > 0){
                        while_array_index--;
                    }
                    _while[while_array_index].end = _functions[i].address;
                    //debug.print(while_array_index);
                    //debug.print(" - ");
                    //debug.println(_while[while_array_index].end);

                }else{
                    address[address_index] = _functions[i].address;
                    address_index++;
                    break;
                }
            }
        }
    }
    // If there isn't any error on functions, we continue with blocks
    if(address_index == 0){
        for( byte i = 1; i <= SLAVES_COUNT; i++ ){
            if(_blocks[i].address){
                if(_blocks[i].type == MODE_WHILE_START){
                    address[address_index] = _blocks[i].address;
                    address_index++;

                    while(_while[while_array_index].start > 0 && while_array_index < (WHILE_LOOPS_COUNT+1)){
                        while_array_index++;
                    }
                    _while[while_array_index].start = _blocks[i].address;
                    //debug.print(while_array_index);
                    //debug.print(" - ");
                    //debug.println(_while[while_array_index].start);
                    
                }
                if(_blocks[i].type == MODE_WHILE_END){
                    if(address_index > 0){
                        address_index--;
                        address[address_index] = 0;

                        while(_while[while_array_index].end > 0 && while_array_index > 0){
                            while_array_index--;
                        }
                        _while[while_array_index].end = _blocks[i].address;
                        //debug.print(while_array_index);
                        //debug.print(" - ");
                        //debug.println(_while[while_array_index].end);
                        
                    }else{
                        address[address_index] = _blocks[i].address;
                        address_index++;
                        break;
                    }
                }
            }
        }
    }
    // Returns the address of the mismatched while block
    return address[0];
}


bool Blocks::slaveExists(byte address){
    bool found = false;
    for( byte i = 1; i <= FUNCTION_COUNT; i++ ){
        if(_functions[i].address == address){
            found = true;
        }else{
            for( byte j = 1; j <= FUNCTION_MODIFIERS_COUNT; j++ ){
                if(_functions[i].modifiers[j].address == address){
                    found = true;
                }
            }
        }
    }
    for( byte i = 1; i <= SLAVES_COUNT; i++ ){
        if(_blocks[i].address == address){
            found = true;
        }else{
            for( byte j = 1; j <= SLAVES_MODIFIERS_COUNT; j++ ){
                if(_blocks[i].modifiers[j].address == address){
                    found = true;
                }
            }
        }
    }

    return found;
}


void Blocks::add_slave(const byte old_address, byte address){
    Wire.beginTransmission(old_address);
    Wire.write(byte(STATE_ADDRESS));        // RegAddress
    Wire.write(byte(address));                    // Value
    Wire.endTransmission();
}


void Blocks::enable_slaves(void){
    digitalWrite(SLAVE_ACTIVATE_PIN, HIGH);
    delay(300);
}


void Blocks::disable_slaves(void){
    digitalWrite(SLAVE_ACTIVATE_PIN, LOW);
    empty_blocks();
    delay(300);
}

void Blocks::enable_function(void){
    digitalWrite(FUNCTION_ACTIVATE_PIN, HIGH);
    delay(300);
}


void Blocks::disable_function(void){
    digitalWrite(FUNCTION_ACTIVATE_PIN, LOW);
    empty_blocks();
    delay(300);
}


void Blocks::open_gate(byte address){
    //debug.print(address);
    //debug.println(F(": Opening."));
    if(!slaveExists(address)){
        debug.println(F(": Doesn't exists."));
    }else{
        Wire.beginTransmission(address);
        Wire.write(byte(STATE_GATE));   // RegAddress
        Wire.write(byte(1));                  // Value
        Wire.endTransmission();
    }
}


void Blocks::close_gate(byte address){
    //debug.print(address);
    //debug.println(F(": Closing."));
    /*if(!slaveExists(address)){
        debug.println(F(": Doesn't exists."));
    }else{*/
        Wire.beginTransmission(address);
        Wire.write(byte(STATE_GATE));   // RegAddress
        Wire.write(byte(0));                  // Value
        Wire.endTransmission();
    //}
}


void Blocks::flash_led(byte address, byte mode){
    /*if(!slaveExists(address)){
        debug.println(address);
        debug.println(F(": Doesn't exists."));
    }else{*/
        Wire.beginTransmission(address);
        Wire.write(byte(STATE_LED));    // RegAddress
        Wire.write(byte(mode));               // Value
        Wire.endTransmission();
    //}
}


void Blocks::read_status(byte address){
    debug.println(F("Slave Status Start -----------------------------"));
    debug.println(address);

    /*if(!slaveExists(address)){
        debug.println(F("Doesn't exists."));
    }else{*/
        //memset(_status,0,sizeof(_status));
        for(int j=0;j<sizeof(_status);j++){
            Wire.requestFrom(address, (uint8_t)1);
            if(Wire.available()){
                byte i = Wire.read();
                debug.print(j);
                debug.print(F(":\t"));
                debug.print(i);
                debug.print(F("\n"));
            }
        }
    //}
    debug.println(F("Slave Status End -------------------------------\n"));
}


void Blocks::set_state(byte address, byte reg, byte value){
    Wire.beginTransmission(address);
    Wire.write(byte(reg));        // RegAddress
    Wire.write(byte(value));      // Value
    Wire.endTransmission();
}


byte Blocks::read_state(byte address, byte reg){
    //memset(_status,0,sizeof(_status));
    boolean received = false;
    for(int j=0;j<sizeof(_status);j++){
        _status[j] = 0;
        Wire.requestFrom(address, (uint8_t)1);
        if(Wire.available()){
            received = true;
            _status[j] = Wire.read();
        }
    }

    return (received) ? _status[reg] : 0;
}

void Blocks::clear_eeprom(byte address){
    Wire.beginTransmission(address);
    Wire.write(byte(255));   // code for eeprom clearing
    Wire.endTransmission();

    debug.print(address);
    debug.println(F(" cleared."));
}
