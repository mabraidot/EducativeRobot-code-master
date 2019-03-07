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

    uint8_t _buffer[RH_NRF24_MAX_MESSAGE_LEN] = {0};
    if (!manager.init()){
        debug.println("RF init failed");
    }

}


void RF::sendMessage(uint8_t *message){

    uint8_t text[RH_NRF24_MAX_MESSAGE_LEN];
    strcpy(text, message);
    
    debug.print("Sending: ");
    debug.println((char *)text);
    if (manager.sendtoWait(text, sizeof(text), RF_CLIENT_ADDRESS)){
        //if (manager.recvfromAckTimeout(_buffer, &_len, 2000, &_from)){
        if(receiveMessageTimeout()){
            debug.print("Response: ");
            debug.println((char *)_buffer);
            sent = true;
        }else{
            debug.println("RF No reply, is nrf24_reliable_datagram_client running?");
            sent = false;
        }
    }else{
        debug.println("RF sendtoWait failed");
        sent = false;
    }

}


boolean RF::receiveMessage(void){

    uint8_t _buffer[RH_NRF24_MAX_MESSAGE_LEN] = {0};

    return manager.recvfromAck(_buffer, &_len, &_from);

}


boolean RF::receiveMessageTimeout(void){

    uint8_t _buffer[RH_NRF24_MAX_MESSAGE_LEN] = {0};

    return manager.recvfromAckTimeout(_buffer, &_len, 2000, &_from);
    
}


byte getNumberFromMessage(uint8_t *message, byte units){

    //byte number = ((byte)message[0]-48)*10 + (byte)message[1]-48; //units=2
    byte number = 0;
    for(byte i = 0; i < units; i++){
        number += ( (byte)message[( units-i-1 )] - 48 ) * pow(10,i);
    }
    return number;

}