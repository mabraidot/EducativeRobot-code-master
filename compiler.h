#ifndef COMPILER_H
#define COMPILER_H

class Compiler {

    public:
        
        Compiler() {};

        void init(void);
        void run(void);
        void scanBlocks(void);

        //DEMO
        int blink_interval = 2000;
        unsigned long blink_timeout = millis() + blink_interval;
        
    private:

        boolean _compiled = false;
        boolean _busy = false;
        byte _queue = 0;
        byte _queue_temp = 0;
        boolean _function_flag = false;
        boolean _steps_flag = false;
        boolean _steps_busy = false;
        byte _loop = 0;

        boolean _addSlave(boolean _function_mode, boolean _modifier_mode, byte _block_address, byte _block_position, byte _block_modifier_position);
        byte _scanI2CBuffer(byte _block_address, boolean _function_mode);
        
        boolean _next(void);
        void _execute(void);
        void _led(byte pin, byte mode);

        byte _get_modifiers_count(boolean _function_flag, byte queue);
        void _set_modifier_values(boolean _function_flag, byte queue, byte i, byte *_type, byte *_loop, byte *_old_value, byte *_address);

};

extern Compiler compiler;

#endif //COMPILER_H