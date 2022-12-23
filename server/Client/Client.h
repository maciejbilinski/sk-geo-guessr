#ifndef __Client__
#define __Client__
#include <exception>
#include <functional>
#include <optional>
#include "../Handler/Handler.h"
#include "../Buffer/WriteBuffer.h"
#include "../Buffer/ReadBuffer.h"

class Server;
class Client : public Handler{    
    private:
        void waitForWrite(bool epollout);
    protected:
        std::optional<std::string> name;
        Server* server;
        ReadBuffer readBuffer;
        std::list<WriteBuffer*> writers;

        void addWriter(WriteBuffer* writer);
        void onRemove(bool send);
    public:
        Client(int fd, Server* server);

        virtual void hookEpoll(int epollFd);
        virtual void handleEvent(unsigned int events);

};


#endif // __Client__
