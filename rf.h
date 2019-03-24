#include <RH_NRF24.h>

#ifndef RF_H
#define RF_H

class RF {

    public:
        
        RF() {};

        void init();
        boolean sendMessage(uint8_t *message, bool ack);
        boolean sendMessage(byte number, bool ack);
        boolean sendMessage(char text[], bool ack);
        boolean receiveMessage(void);
        boolean RF::receiveMessageTimeout(uint16_t timeout);
        byte getNumberFromMessage(byte start, byte units);
        char* getBuffer(void);
        
        int action_wait_interval = 7000;
        unsigned long action_wait_timeout = millis() + action_wait_interval;

        boolean sent = false;

    private:

        uint8_t _buffer[RH_NRF24_MAX_MESSAGE_LEN] = {0};
        uint8_t _len = sizeof(_buffer);
        uint8_t _from;

};

extern RF rf;

#endif //RF_H