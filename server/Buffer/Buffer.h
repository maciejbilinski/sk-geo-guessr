#ifndef __Buffer__
#define __Buffer__

#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <functional>

class Buffer {
    protected:
        char* data;
        ssize_t len;
        ssize_t pos;
        int fd;
        std::function<void(const Buffer&)> onError;

        
        void doube();
        ssize_t remaining();
        char* dataPos();
    public:
        Buffer(int fd, std::function<void(const Buffer&)> onError);
        virtual ~Buffer();
        Buffer(const Buffer&) = delete;

        std::string getData() const;
};

#endif // __Buffer__

