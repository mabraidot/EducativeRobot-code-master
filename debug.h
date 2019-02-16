#ifndef DEBUG_H
#define DEBUG_H

class Debug {

    public:

        Debug() {};

        void print(String message);
        void println(String message);

        void print(byte message);
        void println(byte message);

};

extern Debug debug;

#endif //DEBUG_H