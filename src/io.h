#ifndef INPUTOUTPUT_H
#define INPUTOUTPUT_H

#include <iostream> 
#include <fstream>

#define IO_MODE_IDLE 0
#define IO_MODE_READ 1
#define IO_MODE_WRITE 2

class IOHeader {
    int version;
    int columns;
}

class IOHelper {
    int writingVersion;
    
}

#endif