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


void Blocks::scanI2CDevices(void){

    disable_slaves();
    enable_slaves();

    _finding = true;
    _finding_modifier = true;
    _discovered = 1;
    _data = 0;
    memset(_blocks,0,sizeof(_blocks));
    _block_address = SLAVE_START_ADDRESS;
    _block_position = 0;
    while(_finding){
        // @TODO: make it recursive
        _finding_modifier = true;
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
                if(read_state(_block_address, 3)){
                    _blocks[_block_position] = _block_address;
                    debug.println(F("Opening gate modifier ..."));
                    open_gate(_block_address);
                    delay(300);
                
                    _finding_modifier = true;
                    _block_position++;
                    _block_address++;
                }
            }
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
            if(read_state(_block_address, 3)){
                _blocks[_block_position] = _block_address;
                debug.println(F("Opening gate ..."));
                open_gate(_block_address);
                delay(300);

                _finding = true;
                _block_position++;
                _block_address++;
            }
        }
    }

    scanResults();

}


void Blocks::scanResults(void){
    debug.println(F("\nScanning I2C bus..."));
    for( byte i = 0; i < sizeof(_blocks); i++ ){
        if(_blocks[i]){
            debug.print(i);
            debug.print(F(": "));
            debug.print(_blocks[i]);
            debug.print( (i==0 || i%10) ? F("\t"):F("\n"));
        }
    }
    debug.println(F("\nScanning finished\n"));
}


bool Blocks::slaveExists(byte address){
    bool found = false;
    for( byte i = 0; i < sizeof(_blocks); i++ ){
        if(_blocks[i] == address){
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
    Wire.write(0);        // RegAddress
    Wire.write(address);  // Value
    Wire.endTransmission();
  //}
}


void Blocks::enable_slaves(void){
    digitalWrite(SLAVE_ACTIVATE_PIN, HIGH);
    delay(300);
}


void Blocks::disable_slaves(void){
    digitalWrite(SLAVE_ACTIVATE_PIN, LOW);
    memset(_blocks,0,sizeof(_blocks));
    delay(300);
}


void Blocks::open_gate(byte address){
    if(!slaveExists(address)){
        debug.print(address);
        debug.println(F(": Doesn't exists."));
    }else{
        Wire.beginTransmission(address);
        Wire.write(1);        // RegAddress
        Wire.write(1);        // Value
        Wire.endTransmission();
    }
}


void Blocks::close_gate(byte address){
    if(!slaveExists(address)){
        debug.print(address);
        debug.println(F(": Doesn't exists."));
    }else{
        Wire.beginTransmission(address);
        Wire.write(1);        // RegAddress
        Wire.write(0);        // Value
        Wire.endTransmission();
    }
}


void Blocks::flash_led(byte address){
    if(!slaveExists(address)){
        debug.println(address);
        debug.println(F(": Doesn't exists."));
    }else{
        Wire.beginTransmission(address);
        Wire.write(2);        // RegAddress
        Wire.write(7);        // Value
        Wire.endTransmission();
    }
}


void Blocks::read_status(byte address){
    debug.println(F("\nSlave Status Start -----------------------------"));
    debug.println(address);

    if(!slaveExists(address)){
        debug.println(F("Doesn't exists."));
    }else{
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
    }
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
