#ifndef buffer
#define buffer

#include <cstdlib>
#include <cstring>
#include <unistd.h>
struct Buffer {
        Buffer() {data = (char*) malloc(len);}
        Buffer(const char* srcData, ssize_t srcLen) : len(srcLen) {data = (char*) malloc(len); memcpy(data, srcData, len);}
        ~Buffer() {free(data);}
        Buffer(const Buffer&) = delete;
        void doube() {len*=2; data = (char*) realloc(data, len);}
        ssize_t remaining() {return len - pos;}
        char * dataPos() {return data + pos;}
        char * data;
        ssize_t len = 32;
        ssize_t pos = 0;
    };
#endif
