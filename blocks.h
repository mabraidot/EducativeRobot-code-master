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
        void flash_led(byte address);
        void read_status(byte address);
        uint8_t read_state(byte address, byte reg);

    private:

        byte _blocks[255]        = {0};
        uint8_t _status[4];
        bool _finding;
        bool _finding_modifier;
        uint8_t _discovered;
        byte _rc;
        byte _data;
        byte _block_address;
        byte _block_position;

};

extern Blocks blocks;

#endif //BLOCKS_H