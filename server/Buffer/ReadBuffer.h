#ifndef __ReadBuffer__
#define __ReadBuffer__

#include <functional>
#include "Buffer.h"
#include "../Packet/Packet.h"

class ReadBuffer : public Buffer{
    protected:
        std::function<void(const Packet&)> onPacket;

    public:
        ReadBuffer(int fd, std::function<void(const Buffer&)> onError, std::function<void(const Packet&)> onPacket);
        ReadBuffer(const ReadBuffer&) = delete;

        void read();
};

#endif // __ReadBuffer__

