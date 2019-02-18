#include <Arduino.h>
#include <Wire.h>
extern "C" {
  #include "utility/twi.h"
}
#include "config.h"
#include "debug.h"
#include "blocks.h"

Blocks blocks;


void Blocks::init(void){

    Wire.begin();

    pinMode(SLAVE_ACTIVATE_PIN, OUTPUT);
    digitalWrite(SLAVE_ACTIVATE_PIN, LOW);

    pinMode(FUNCTION_ACTIVATE_PIN, OUTPUT);
    digitalWrite(FUNCTION_ACTIVATE_PIN, LOW);

}


void Blocks::_empty_blocks(void){
    memset(_blocks,0,SLAVES_COUNT);
}


void Blocks::scanI2CDevices(void){

    disable_slaves();
    enable_slaves();

    bool _finding = true;
    bool _finding_modifier = true;
    byte _rc = 1;
    byte _data = 0;
    //memset(_blocks,0,SLAVES_COUNT);
    _empty_blocks();
    byte _block_address = SLAVE_START_ADDRESS;
    byte _block_position = 0;
    byte _block_modifier_position = 0;
    while(_finding){
        // @TODO: make it recursive
        _finding_modifier = true;
        _block_modifier_position = 0;
        while(_finding_modifier){
            _finding_modifier = false;
            _rc = twi_writeTo(SLAVE_MODIFIER_ADDRESS, &_data, 0, 1, 0);
            if(_rc == 0){       // Block found
                //give it an address
                debug.println(_block_address);
                debug.println(F("Adding slave modifier ..."));
                add_slave(SLAVE_MODIFIER_ADDRESS, _block_address);
                delay(600);
                // If modifier was activated successfully, open its gate
                if(read_state(_block_address, SLAVE_STATE_ACTIVATED)){
                    _blocks[_block_position].modifiers[_block_modifier_position].address = _block_address;
                    _blocks[_block_position].modifiers[_block_modifier_position].type = read_state(_block_address, SLAVE_STATE_FUNCTION);
                    _blocks[_block_position].modifiers[_block_modifier_position].value = read_state(_block_address, SLAVE_STATE_VALUE);
                    open_gate(_block_address);
                    delay(300);
                    flash_led(_blocks[_block_position].modifiers[_block_modifier_position].address,1);
                
                    _finding_modifier = true;
                    _block_modifier_position++;
                    _block_address++;
                }
            }
        }

        if(_blocks[0].address){
            _block_position++;
        }
        _finding = false;
        _rc = twi_writeTo(SLAVE_ADDRESS, &_data, 0, 1, 0);
        if(_rc == 0){       // Block found
            //give it an address
            debug.println(_block_address);
            debug.println(F("Adding slave ..."));

            add_slave(SLAVE_ADDRESS, _block_address);
            delay(600);
            // If slave was activated successfully, open its gate
            if(read_state(_block_address, SLAVE_STATE_ACTIVATED)){
                _blocks[_block_position].address = _block_address;
                _blocks[_block_position].type = read_state(_block_address, SLAVE_STATE_FUNCTION);
                _blocks[_block_position].value = read_state(_block_address, SLAVE_STATE_VALUE);
                open_gate(_block_address);
                delay(300);
                flash_led(_blocks[_block_position].address,1);

                _finding = true;
                _block_address++;
            }
        }
    }

    scanResults();
    delay(1000);
    off_leds();
}


void Blocks::off_leds(){
    for( byte i = 0; i < SLAVES_COUNT; i++ ){
        if(_blocks[i].address){
            flash_led(_blocks[i].address,0);
            for( byte j = 0; j < SLAVES_MODIFIERS_COUNT; j++ ){
                if(_blocks[i].modifiers[j].address){
                    flash_led(_blocks[i].modifiers[j].address,0);
                }
            }
        }
    }
}


void Blocks::scanResults(void){
    debug.println(F("\nScanning I2C bus..."));
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
    Wire.write(SLAVE_STATE_ADDRESS);        // RegAddress
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
    //memset(_blocks,0,SLAVES_COUNT);
    _empty_blocks();
    delay(300);
}


void Blocks::open_gate(byte address){
    debug.print(address);
    debug.println(F(": Opening."));
    /*if(!slaveExists(address)){
        debug.println(F(": Doesn't exists."));
    }else{*/
        Wire.beginTransmission(address);
        Wire.write(SLAVE_STATE_GATE);   // RegAddress
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
        Wire.write(SLAVE_STATE_GATE);   // RegAddress
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
        Wire.write(SLAVE_STATE_LED);    // RegAddress
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

    debug.println(F("\nReg State Start -----------------------------"));
    debug.print(reg);
    debug.print(F("\t"));
    debug.print(_status[reg]);
    debug.println(F("\nReg State End -------------------------------\n"));

    return _status[reg];
}
