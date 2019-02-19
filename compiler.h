#ifndef COMPILER_H
#define COMPILER_H

class Compiler {

    public:
        Compiler() {};

        void init(void);
        boolean readRunButton(void);
        boolean readStepsButton(void);
        void run(void);
        void runSteps(void);
        
    private:
        boolean _buttonInput = true;        /* 0 or 1 depending on the input signal */
        unsigned int _buttonIntegrator = 0; /* Will range from 0 to the specified MAXIMUM */
        boolean _buttonOutput = true;       /* Cleaned-up version of the input signal */
        
        boolean _readButton(uint8_t button_pin);
        boolean _addSlave(boolean _function_mode, boolean _modifier_mode, byte _block_address, byte _block_position, byte _block_modifier_position);
        byte _scanI2CBuffer(byte _block_address, boolean _function_mode);
        void _scanBlocks(void);

};

extern Compiler compiler;

#endif //COMPILER_H