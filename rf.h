#ifndef RF_H
#define RF_H

class RF {

    public:
        
        RF() {};

        void init();
        void sendMessage(uint8_t *message);
        boolean receiveMessage(void);
        boolean receiveMessageTimeout(void);
        byte getNumberFromMessage(uint8_t *message, byte units);
        uint8_t getMessageFromNumber(byte number);
        
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