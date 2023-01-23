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
        double coords[2];
        std::string team_affilation;

        Server* server;
        ReadBuffer readBuffer;
        std::list<WriteBuffer*> writers;

    public:
        void onRemove(bool send);
        void removeTeam();
        void addWriter(WriteBuffer* writer);
        void setName(std::string);
        std::string getName();
        std::string getTeamName();
        Client(int fd, Server* server);
        virtual ~Client();

        virtual void hookEpoll(int epollFd);
        virtual void handleEvent(unsigned int events);

};


#endif // __Client__
