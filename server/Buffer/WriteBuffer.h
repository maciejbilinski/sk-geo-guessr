#ifndef __WriteBuffer__
#define __WriteBuffer__

#include <functional>
#include "Buffer.h"
#include "../Packet/Packet.h"

class WriteBuffer : public Buffer{
    protected:
        std::function<void()> onDone;
    public:
        WriteBuffer(int fd, std::function<void(const Buffer&)> onError, std::function<void()> onDone, const Packet& packet);
        WriteBuffer(const WriteBuffer&) = delete;

        bool write();
};

#endif // __WriteBuffer__

