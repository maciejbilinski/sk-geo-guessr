#include "Packet.h"

#include "../exceptions/PackageParsingFailed.h"
#include <cstdio>
#include <cstring>
#include <stdexcept>
#include <string>
Packet::Packet(){
    this->state = -1;
}

Packet::Packet(int game_state,char *action,char * content){
    this->state = game_state;
    strcpy(this->action,action);
    strcpy(this->content,content);
}
Packet::~Packet(){

}
int Packet::fromBuforToPacket(int fd,Buffer *readBuffer){
    ssize_t count = read(fd, readBuffer->dataPos(), readBuffer->remaining());
    if(count <= 0)
        throw PackageParsingFailed();
    else {
        readBuffer->pos += count;
        char * eol = (char*) memchr(readBuffer->data, ';', readBuffer->pos);
        if(eol == nullptr) {
            if(0 == readBuffer->remaining())
                readBuffer->doube();
        } else {
            do {
                auto thismsglen = eol - readBuffer->data + 1;
                char *temp;
                temp = strdup(readBuffer->data);
                temp[thismsglen-1]=0;
                char *key,*value;
                if((key= strsep(&temp,":"))==NULL)
                    return -1;
                if((value= strsep(&temp,":"))==NULL)
                    return -1;
                if(strcmp(key,"state")==0){
                    printf("state:%s\n",value);
                    try{
                        this->state=std::stoi(value);
                    }catch(std::invalid_argument &e){
                        return -1;
                    }
                }else if (strcmp(key,"action")==0) {
                    printf("action:%s\n",value);
                    this->action=value;
                }else if(strcmp(key,"content")==0){
                    printf("content:%s\n",value);
                    this->content=value;
                }else{
                    return -1;
                }
                auto nextmsgslen =  readBuffer->pos - thismsglen;
                memmove(readBuffer->data, eol+1, nextmsgslen);
                readBuffer->pos = nextmsgslen;
            } while((eol = (char*) memchr(readBuffer->data, ';', readBuffer->pos)));
        }
    }
    return 0;
}

void Packet::print(){
    printf("s:%d o:%s c:%s\n",this->state,this->action,this->content);
}
