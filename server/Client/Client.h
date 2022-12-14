#ifndef __Client__
#define __Client__
#include <exception>
#include <functional>
#include "../Packet/Packet.h"
#include "../Handler/Handler.h"

class Client : public Handler{
    void readPackageFromFileDescriptor();
    char *name;
    enum team{green=0,yellow=1,orange=2,pink=3};
    protected:
        std::function<void(Client*)> onRemove;
    public:
        Client(int fd, std::function<void(Client*)> onRemove);

        virtual void hookEpoll(int epollFd);
        virtual void handleEvent(unsigned int events);
};


#endif // __Client__
