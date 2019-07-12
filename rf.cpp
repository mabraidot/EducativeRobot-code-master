#include <Arduino.h>
#include "config.h"
#include "debug.h"
#include "rf.h"
#include <RHReliableDatagram.h>
#include <RH_NRF24.h>
#include <SPI.h>


RH_NRF24 driver(RF_CE, RF_CSN);
RHReliableDatagram manager(driver, RF_SERVER_ADDRESS);



void RF::init(void){

    if (!manager.init()){
        debug.println(F("RF init failed"));
    }

}


boolean RF::sendMessage(uint8_t *message, bool ack){

    uint8_t text[RH_NRF24_MAX_MESSAGE_LEN] = {0};
    strcpy(text, message);
    
    debug.print(F("RF Sending: "));
    debug.println((char *)text);
    sent = false;
    if (manager.sendtoWait((uint8_t *)text, sizeof(text), RF_CLIENT_ADDRESS)){
        if(ack){
            if(receiveMessageTimeout(2000)){
                debug.print(F("RF ACK Response: "));
                debug.println((char *)_buffer);
                if(strcmp(_buffer, "ACK") == 0){
                    sent = true;
                }else{
                    debug.println(F("RF No ACK reply"));
                    sent = false;
                }
            }else{
                debug.println(F("RF No reply, is nrf24_reliable_datagram_client running?"));
                sent = false;
            }
        }else{
            sent = true;
        }
    }else{
        debug.println(F("RF sendtoWait failed"));
        sent = false;
    }
    return sent;
}


boolean RF::sendMessage(byte number, byte value, bool ack){

    uint8_t message[RH_NRF24_MAX_MESSAGE_LEN] = {0};
    sprintf(message, "%02d%02d", number, value);
    debug.print(F("Sending char: "));
    debug.println((char *)message);
    return sendMessage(message, ack);
}


boolean RF::sendMessage(char text[], bool ack){

    uint8_t message[RH_NRF24_MAX_MESSAGE_LEN] = {0};
    sprintf(message, "%s", text);

    return sendMessage(message, ack);
}


boolean RF::receiveMessage(void){

    memset(_buffer, 0, _len);
    return manager.recvfromAck(_buffer, &_len, &_from);

}


boolean RF::receiveMessageTimeout(uint16_t timeout){

    memset(_buffer, 0, _len);
    return manager.recvfromAckTimeout(_buffer, &_len, timeout, &_from);
    
}


char* RF::getBuffer(){
    return (char*)_buffer;
}


byte RF::getNumberFromMessage(byte start, byte units){

    //byte number = (message[0]-48)*10 + (message[1]-48)*1; //start=0, units=2
    byte number = 0;
    for(byte i = 0; i < units; i++){
        number += ( (byte)_buffer[( units - i - 1 + start )] - 48 ) * pow(10,i);
    }
    return number;

}