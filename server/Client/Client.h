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
        std::string name;
        int team_affilation;
        double coords[2];

        Server* server;
        ReadBuffer readBuffer;
        std::list<WriteBuffer*> writers;

        void addWriter(WriteBuffer* writer);
        void onRemove(bool send);
    public:
        void setName(std::string);
        Client(int fd, Server* server);

        virtual void hookEpoll(int epollFd);
        virtual void handleEvent(unsigned int events);

};


#endif // __Client__
