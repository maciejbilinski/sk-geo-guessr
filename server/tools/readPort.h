#ifndef __readPort__
#define __readPort__

#include <cstdlib>
#include "../exceptions/IncorrectPort.h"

long readPort(const char* txt){
    char* ptr;
    long port = strtol(txt, &ptr, 10);
    if(*ptr!=0 || port<1 || (port>65535)){
        throw IncorrectPort();
    }
    return port;
}

#endif