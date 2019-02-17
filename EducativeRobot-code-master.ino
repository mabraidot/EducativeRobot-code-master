#include "config.h"
#include "debug.h"
#include "blocks.h"


void process_serial(){
  char cmd = Serial.read();
  byte address;
  if (cmd > 'Z') cmd -= 32;
  switch (cmd) {
    case 'H': help(); break;
    case 'S': blocks.scanI2CDevices(); break;
    case 'M': blocks.scanResults(); break;
    case 'D': blocks.disable_slaves(); break;
    case 'L': 
        {
            address = Serial.parseInt();
            byte mode = Serial.parseInt();
            blocks.flash_led(address, mode); 
        }
        break;
    case 'E': 
        {
            address = Serial.parseInt();
            byte reg = Serial.parseInt();
            blocks.read_state(address, reg); 
        }
        break;
    case 'R': 
        {
            address = Serial.parseInt();
            blocks.read_status(address); 
        }
        break;
    case 'C': 
        {
            address = Serial.parseInt();
            blocks.close_gate(address); 
        }
        break;
    case 'O': 
        {
            address = Serial.parseInt();
            blocks.open_gate(address); 
        }
        break;
  }
  
  while (Serial.read() != 10); // dump extra characters till LF is seen (you can use CRLF or just LF)

}


void help(){
  debug.println(F("\nI2C CONSOLE INTERFACE"));
  debug.println(F("Available commands:"));
  debug.println(F(""));
  debug.println(F("H: This help"));
  debug.println(F("S: Scan for I2C slaves"));
  debug.println(F("M: Show list of slaves found"));
  debug.println(F("D: Disable all slaves"));
  debug.println(F("R<addr>: Read the buffer status of slave on <addr> address"));
  debug.println(F("E<addr>,<pos>: Read <pos> state of slave on <addr> address"));
  debug.println(F("L<addr>,<mode>: Onboard led of slave on <addr> address. Modes: 0->off, 1->on, 2->blink"));
  debug.println(F("O<addr>: Activate the child slave of the slave on <addr> address"));
  debug.println(F("C<addr>: Deactivate all the children slaves of the slave on <addr> address"));
}


void setup()
{
  Serial.begin(115200);

  blocks.init();
  
  // wait for slave to finish any init sequence
  delay(2000);
  
}

void loop()
{

  if (Serial.available()) process_serial();

}
