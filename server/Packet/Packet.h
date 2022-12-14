#ifndef __Packet__
#define __Packet__

#include <string>
#include "../Buffer.h"
        enum game_state{intro=0,voting=1,admin_panel=2,game=3,status=4};
class Packet{
    public:
        int state;
        char *action;
        char * content;

        Packet(int game_state,char *action,char * content);
        Packet();
        ~Packet();
        void print();
        int fromBuforToPacket(int fd,Buffer *readBuffer);
};
#endif

