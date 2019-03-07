#include "config.h"
#include "debug.h"
#include "blocks.h"
#include "compiler.h"
#include "rf.h"

Blocks blocks;
Compiler compiler;
RF rf;

/****************************************/
/*  RF                                  */
/****************************************/
/*#include <RHReliableDatagram.h>
#include <RH_NRF24.h>
#include <SPI.h>

#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2

RH_NRF24 driver(9, 10);

RHReliableDatagram manager(driver, SERVER_ADDRESS);
uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN] = {0};*/

/****************************************/
/*  End RF                              */
/****************************************/

void process_serial(){
  char cmd = Serial.read();
  byte address;
  if (cmd > 'Z') cmd -= 32;
  switch (cmd) {
    case 'H': help(); break;
    case 'S': compiler.scanBlocks(); break;
    case 'M': blocks.scanResults(); break;
    case 'D': 
        {
            blocks.disable_slaves(); 
            blocks.disable_function(); 
        }
        break;
    case 'L': 
        {
            address = Serial.parseInt();
            byte mode = Serial.parseInt();
            blocks.flash_led(address, mode); 
        }
        break;
    case 'V': 
        {
            address = Serial.parseInt();
            byte reg = Serial.parseInt();
            byte value = Serial.parseInt();
            blocks.set_state(address, reg, value); 
        }
        break;
    case 'E': 
        {
            address = Serial.parseInt();
            byte reg = Serial.parseInt();
            blocks.read_state(address, reg); 
        }
        break;
    case 'B': 
        {
            address = Serial.parseInt();
            blocks.clear_eeprom(address); 
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
  debug.println(F("B<addr>: Clear EEPROM of slave on <addr> address"));
  debug.println(F("R<addr>: Read the buffer status of slave on <addr> address"));
  debug.println(F("E<addr>,<pos>: Read <pos> state of slave on <addr> address"));
  debug.println(F("L<addr>,<mode>: Onboard led of slave on <addr> address. Modes: 0->off, 1->on, 2->blink"));
  debug.println(F("O<addr>: Activate the child slave of the slave on <addr> address"));
  debug.println(F("C<addr>: Deactivate all the children slaves of the slave on <addr> address"));
  debug.println(F("V<addr>,<reg>,<val>: Set <val> for the <reg> state on <addr> address."));
}


void setup()
{
    Serial.begin(115200);

    blocks.init();
    compiler.init();
    rf.init();
    // RF
    /*if (!manager.init()){
        debug.println("RF init failed");
    }else{
        debug.println("RF init OK");
    }*/

    // wait for slave to finish any init sequence
    delay(2000);
  
}

void loop()
{
    if (Serial.available()) process_serial();

    /*static int action_interval = 7000;
    static unsigned long action_timeout = millis() + action_interval;
    static uint8_t message[RH_NRF24_MAX_MESSAGE_LEN] = "01";
    static boolean sent = false;
    
    uint8_t len = sizeof(buf);
    uint8_t from;

    //if(!sent && strncmp(message, "", 1) != 0){
    if(!sent){

        //sent = send_message(message);
        action_timeout = millis() + action_interval;
        Serial.print("Sending: ");
        Serial.println((char *)message);
        if (manager.sendtoWait(message, sizeof(message), CLIENT_ADDRESS)){
            sent = true;
            if (manager.recvfromAckTimeout(buf, &len, 2000, &from)){
                Serial.print("Response: ");
                Serial.println((char *)buf);
            }else{
                Serial.println("No reply, is nrf24_reliable_datagram_server running?");
                sent = false;
            }
        }else{
            Serial.println("RF sendtoWait failed");
            sent = false;
        }

    }else{
        if(sent){
            if (manager.recvfromAck(buf, &len, &from)){
                Serial.print("Response finished: ");
                Serial.println((char *)buf);
                sent = false;
                byte action = ((byte)message[0]-48)*10 + (byte)message[1]-48;
                action++;
                sprintf(message, "%02d", action);
            }else{
                if(action_timeout < millis()){
                    action_timeout = millis() + action_interval;
                    Serial.println("Nothing received");
                    sent = false;
                }
            }
        }
    }*/
    
    
    /*static int action_interval = 4000;
    static unsigned long action_timeout = millis() + action_interval;
    static uint8_t action = 0;
    static boolean sent = false;

    uint8_t len = sizeof(buf);
    uint8_t from;
    if(action == 0){
        if (manager.recvfromAck(buf, &len, &from)){
            Serial.print("Buffer: ");
            Serial.println((char *)buf);
            //action = (byte)buf[0] - 48;
            action = ((byte)buf[0]-48)*10 + (byte)buf[1]-48;
            Serial.print("Action: ");
            Serial.println(action);
            if(!manager.sendtoWait(buf, sizeof(buf), CLIENT_ADDRESS)){
                Serial.println("RF sendtoWait response failed");
            }
            action_timeout = millis() + action_interval;
        }
    }
    
    if(action > 0){
        if(action_timeout < millis()){
            action_timeout = millis() + action_interval;
            //uint8_t message[] = "FF-1";
            uint8_t message[RH_NRF24_MAX_MESSAGE_LEN];
            sprintf(message, "FF-%02d", action);

            Serial.println((char *)message);
            if(!manager.sendtoWait((uint8_t *)message, sizeof(message), CLIENT_ADDRESS)){
                Serial.println("RF sendtoWait failed");
            }
            action = 0;
        }
    }*/


    compiler.run();
    
}
