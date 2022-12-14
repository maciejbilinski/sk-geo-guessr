#ifndef __Server__
#define __Server__
#include <exception>
#include "../Handler/Handler.h"
#include "../Client/Client.h"
#include <unordered_set>
#include <vector>

class Server : public Handler{
    protected:
        bool isStarted;
        std::unordered_set<Client*> clients;
        bool log;
        //std::vector<User> users;
    public:
        Server(bool log=true);
        ~Server();
        bool start(long port);
        bool close();

        virtual void hookEpoll(int epollFd);

        virtual void handleEvent(unsigned int events);
};


#endif // __Server__
