#ifndef BLOCKS_H
#define BLOCKS_H

class Blocks {

    public:

        Blocks() {};

        void init(void);
        void scanI2CDevices(void);
        void scanResults(void);
        bool slaveExists(byte address);

        void add_slave(const byte old_address, byte address);
        void enable_slaves(void);
        void disable_slaves(void);

        void open_gate(byte address);
        void close_gate(byte address);
        void flash_led(byte address, byte mode);
        void read_status(byte address);
        uint8_t read_state(byte address, byte reg);

    private:

        typedef struct {
            byte address;
            byte type;
            byte value;
        } _functionModifier;

        typedef struct {
            byte address;
            byte type;
            _functionModifier modifiers[FUNCTION_MODIFIERS_COUNT];
        } _functionBlock;

        _functionBlock _functions[FUNCTION_COUNT];

        typedef struct {
            byte address;
            byte type;
            byte value;
        } _slaveModifier;

        typedef struct {
            byte address;
            byte type;
            _slaveModifier modifiers[SLAVES_MODIFIERS_COUNT];
        } _slaveBlock;

        _slaveBlock _blocks[SLAVES_COUNT];
        //byte _blocks[128]        = {0};

        void _empty_blocks(void);

        /**
         * Array of state values from slaves
         * Pos 0 -> Set new I2C address
         * Pos 1 -> Activate any child slave
         * Pos 2 -> Flash the LED. Posible values:
         *          0 -> Off
         *          1 -> On
         *          2 -> Blink
         * Pos 3 -> Activated block
         * Pos 4 -> Slave function. e.g. Forward arrow
         * Pos 5 -> Slave modifying value
         */
        uint8_t _status[6];

};

extern Blocks blocks;

#endif //BLOCKS_H