#ifndef RF_H
#define RF_H

class RF {

    public:
        
        RF() {};

        void init();
        boolean sendMessage(uint8_t *message);
        boolean sendMessage(byte number);
        boolean receiveMessage(void);
        boolean RF::receiveMessageTimeout(uint16_t timeout);
        byte getNumberFromMessage(uint8_t *message, byte units);
        
        int action_wait_interval = 7000;
        unsigned long action_wait_timeout = millis() + action_wait_interval;

        boolean sent = false;

    private:

        uint8_t _buffer;
        uint8_t _len = sizeof(_buffer);
        uint8_t _from;

};

extern RF rf;

#endif //RF_H