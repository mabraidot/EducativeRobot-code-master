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
    for( byte i = 0; i < FUNCTION_COUNT; i++ ){
        for( byte j = 0; j < FUNCTION_MODIFIERS_COUNT; j++ ){
            _functions[i].modifiers[j] = {};
        }
        _functions[i] = {};
    }
    for( byte i = 0; i < SLAVES_COUNT; i++ ){
        for( byte j = 0; j < SLAVES_MODIFIERS_COUNT; j++ ){
            _blocks[i].modifiers[j] = {};
        }
        _blocks[i] = {};
    }
}


void Blocks::off_leds(){
    for( byte i = 0; i < FUNCTION_COUNT; i++ ){
        if(_functions[i].address){
            flash_led(_functions[i].address,STATE_LED_OFF);
            for( byte j = 0; j < FUNCTION_MODIFIERS_COUNT; j++ ){
                if(_functions[i].modifiers[j].address){
                    flash_led(_functions[i].modifiers[j].address,STATE_LED_OFF);
                }
            }
        }
    }
    for( byte i = 0; i < SLAVES_COUNT; i++ ){
        if(_blocks[i].address){
            flash_led(_blocks[i].address,STATE_LED_OFF);
            for( byte j = 0; j < SLAVES_MODIFIERS_COUNT; j++ ){
                if(_blocks[i].modifiers[j].address){
                    flash_led(_blocks[i].modifiers[j].address,STATE_LED_OFF);
                }
            }
        }
    }
}


void Blocks::scanResults(void){
    debug.println(F("\nScanning I2C bus..."));
    debug.println(F("FUNCTION"));
    for( byte i = 0; i < FUNCTION_COUNT; i++ ){
        if(_functions[i].address){
            debug.print(i);
            debug.print(F(": "));
            debug.println(_functions[i].address);
            for( byte j = 0; j < FUNCTION_MODIFIERS_COUNT; j++ ){
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
    for( byte i = 0; i < SLAVES_COUNT; i++ ){
        if(_blocks[i].address){
            debug.print(i);
            debug.print(F(": "));
            debug.println(_blocks[i].address);
            for( byte j = 0; j < SLAVES_MODIFIERS_COUNT; j++ ){
                if(_blocks[i].modifiers[j].address){
                    debug.print(F("\t\t"));
                    debug.print(j);
                    debug.print(F(": "));
                    debug.println(_blocks[i].modifiers[j].address);
                }
            }
        }
    }
    debug.println(F("\nScanning finished\n"));
}


bool Blocks::slaveExists(byte address){
    bool found = false;
    for( byte i = 0; i < FUNCTION_COUNT; i++ ){
        if(_functions[i].address == address){
            found = true;
        }
    }
    for( byte i = 0; i < SLAVES_COUNT; i++ ){
        if(_blocks[i].address == address){
            found = true;
        }
    }

    return found;
}


void Blocks::add_slave(const byte old_address, byte address){
  /*if(!slaveExists(address)){
      debug.print(address);
      debug.println(F(": Doesn't exists."));
  }else{*/
    Wire.beginTransmission(old_address);
    Wire.write(STATE_ADDRESS);        // RegAddress
    Wire.write(address);                    // Value
    Wire.endTransmission();
  //}
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
    debug.print(address);
    debug.println(F(": Opening."));
    /*if(!slaveExists(address)){
        debug.println(F(": Doesn't exists."));
    }else{*/
        Wire.beginTransmission(address);
        Wire.write(STATE_GATE);   // RegAddress
        Wire.write(1);                  // Value
        Wire.endTransmission();
    //}
}


void Blocks::close_gate(byte address){
    debug.print(address);
    debug.println(F(": Closing."));
    /*if(!slaveExists(address)){
        debug.println(F(": Doesn't exists."));
    }else{*/
        Wire.beginTransmission(address);
        Wire.write(STATE_GATE);   // RegAddress
        Wire.write(0);                  // Value
        Wire.endTransmission();
    //}
}


void Blocks::flash_led(byte address, byte mode){
    /*if(!slaveExists(address)){
        debug.println(address);
        debug.println(F(": Doesn't exists."));
    }else{*/
        Wire.beginTransmission(address);
        Wire.write(STATE_LED);    // RegAddress
        Wire.write(mode);               // Value
        Wire.endTransmission();
    //}
}


void Blocks::read_status(byte address){
    debug.println(F("Slave Status Start -----------------------------"));
    debug.println(address);

    /*if(!slaveExists(address)){
        debug.println(F("Doesn't exists."));
    }else{*/
        memset(_status,0,sizeof(_status));
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


uint8_t Blocks::read_state(byte address, byte reg){
    memset(_status,0,sizeof(_status));
    for(int j=0;j<sizeof(_status);j++){
        Wire.requestFrom(address, (uint8_t)1);
        if(Wire.available()){
            _status[j] = Wire.read();
        }
    }

    /*debug.println(F("\nReg State Start -----------------------------"));
    debug.print(reg);
    debug.print(F("\t"));
    debug.print(_status[reg]);
    debug.println(F("\nReg State End -------------------------------\n"));
    */
    return _status[reg];
}
