#ifndef __Client__
#define __Client__
#include <exception>
#include <functional>
#include "../Handler/Handler.h"

class Client : public Handler{
    protected:
        std::function<void(Client*)> onRemove;
    public:
        Client(int fd, std::function<void(Client*)> onRemove);

        virtual void hookEpoll(int epollFd);

        virtual void handleEvent(unsigned int events);
};


#endif // __Client__
