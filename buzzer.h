#ifndef BUZZER_H
#define BUZZER_H

class Buzzer {

    public:

        Buzzer() {};

        void init(void);
        void startUp(void);
        void error(void);

        void compilationOk(void);
        void blockExecutionBegining(void);
        void blockExecutionRunning(void);
        void executionEnd(void);

    private:
        void _note(byte index, uint16_t frequency, uint16_t duration);

        byte _melody_index = 0;
        unsigned long _duration_timeout = 0;
        unsigned long _current_time = 0;
        unsigned long _previous_time = 0;

};

extern Buzzer buzzer;

#endif //BUZZER_H