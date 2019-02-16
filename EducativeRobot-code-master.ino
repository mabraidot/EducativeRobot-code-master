#include "config.h"
#include "debug.h"
#include "blocks.h"


void help(){
  debug.println(F("\nI2C CONSOLE INTERFACE"));
  debug.println(F("Available commands:"));
  debug.println(F(""));
  debug.println(F("H: This help"));
  debug.println(F("S: Scan for I2C slaves"));
  debug.println(F("M: Show list of slaves found"));
  debug.println(F("D: Disable all slaves"));
  debug.println(F("R<addr>: Read the buffer status of slave on <addr> address"));
  debug.println(F("L<addr>: Flash the onboard led of slave on <addr> address"));
  debug.println(F("O<addr>: Activate the child slave of the slave on <addr> address"));
  debug.println(F("C<addr>: Deactivate all the children slaves of the slave on <addr> address"));
}

void process_serial(){
  char cmd = Serial.read();
  if (cmd > 'Z') cmd -= 32;
  switch (cmd) {
    case 'H': help(); break;
    case 'S': blocks.scanI2CDevices(); break;
    case 'M': blocks.scanResults(); break;
    case 'D': blocks.disable_slaves(); break;
    case 'L': 
        blocks.flash_led(Serial.parseInt()); 
        break;
    case 'O': 
        blocks.open_gate(Serial.parseInt()); 
        break;
    case 'C': 
        blocks.close_gate(Serial.parseInt()); 
        break;
    case 'R': 
        blocks.read_status(Serial.parseInt()); 
        break;
  }
  
  while (Serial.read() != 10); // dump extra characters till LF is seen (you can use CRLF or just LF)

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
