#include "WriteBuffer.h"
#include <sys/socket.h>
#include <cstdlib>
#include <cstring>

WriteBuffer::WriteBuffer(int fd, std::function<void(const Buffer&)> onError, std::function<void()> onDone, const Packet& packet): onDone{onDone}, Buffer(fd, onError) {
    std::string str = packet.toString();
    len = str.size();
    data = (char*) malloc(len); 
    memcpy(data, str.c_str(), len);
}

bool WriteBuffer::write(){
    int sent = send(fd, data, len, MSG_DONTWAIT);
    if(sent == len){
        this->onDone();
        return true;
    }

    if(sent == -1) {
        if(errno != EWOULDBLOCK && errno != EAGAIN){
            onError(*this);
        }
    } else {
        memmove(data, data+sent, len-sent+1);
    }
    return false;
}