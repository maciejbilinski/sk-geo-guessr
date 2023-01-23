#include "ReadBuffer.h"
#include <sys/socket.h>
#include <cstdlib>
#include <cstring>

ReadBuffer::ReadBuffer(int fd, std::function<void(const Buffer&)> onError, std::function<void(const Packet&)> onPacket): onPacket{onPacket}, Buffer(fd, onError) {

}

void ReadBuffer::read(){
    ssize_t count = ::read(fd, dataPos(), remaining()); // odczytaj  maksymalnie remaining znaków
    if(count <= 0)
        this->onError(*this);
    else { // coś tam przyszło
        pos += count; // pos to tak naprawdę indeks za ostatnim znakiem
        char* eol = (char*) memchr(data, '\n', pos); // zwraca wsk na znak nowej linii
        if(eol == nullptr) { // nie ma nowej linii - niepełna wiadomość, może reszta przyjdzie później
            if(0 == remaining()) // jeżeli nie ma miejsca na resztę wiadomości, więc zwiększam bufor
                doube();
        } else { // znaleziono co najmniej jeden potencjalny pakiet
            do {
                auto thismsglen = eol - data;
                std::string fullMsg(data, thismsglen);
                try{
                    Packet packet(fullMsg);
                    this->onPacket(packet);
                }catch(const std::exception& e){
                    std::cout << "Parsing packet error" << std::endl;
                    this->onError(*this);
                }
                auto nextmsgslen =  pos - thismsglen - 1;
                memmove(data, eol+1, nextmsgslen);  
                pos = nextmsgslen;
            } while((eol = (char*) memchr(data, '\n', pos)));
        }
    }
}
