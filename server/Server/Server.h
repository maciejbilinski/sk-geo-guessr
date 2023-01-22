#ifndef __Server__
#define __Server__
#include <exception>
#include "../Handler/Handler.h"
#include "../Buffer/WriteBuffer.h"
#include "../Client/Client.h"
#include "../Game/Game.h"
#include <unordered_set>
#include <vector>
#include <list>

class Server : public Handler{
    protected:
        std::unordered_set<Client*> clients;
        bool log;
    public:
        Server(long port, bool log=true);
        virtual ~Server();

        virtual void hookEpoll(int epollFd);

        virtual void handleEvent(unsigned int events);

        void onClientRemove(Client* client);

        Game geoguessrGame;
};


#endif // __Server__
