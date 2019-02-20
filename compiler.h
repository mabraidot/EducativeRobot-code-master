#ifndef COMPILER_H
#define COMPILER_H

class Compiler {

    public:
        
        Compiler() {};

        void init(void);
        void run(void);
        
    private:
        
        boolean _compiled = false;
        boolean _busy = false;
        byte _queue = 0;
        byte _queue_temp = 0;
        boolean _function_flag = false;
        
        boolean _addSlave(boolean _function_mode, boolean _modifier_mode, byte _block_address, byte _block_position, byte _block_modifier_position);
        byte _scanI2CBuffer(byte _block_address, boolean _function_mode);
        void _scanBlocks(void);

};

extern Compiler compiler;

#endif //COMPILER_H