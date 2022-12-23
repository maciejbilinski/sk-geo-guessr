#include "Buffer.h"
#include <cstdlib>

Buffer::Buffer(int fd, std::function<void(const Buffer&)> onError) : fd{fd}, onError{onError} {
    this->len = 32;
    this->pos = 0;
    this->data = (char*) malloc(this->len);
}

Buffer::~Buffer(){
    free(data);
}

void Buffer::doube() {
    this->len *= 2;
    this->data = (char*) realloc(data, len);
}
ssize_t Buffer::remaining() {
    return len - pos;
}

char* Buffer::dataPos(){
    return data + pos;
}

std::string Buffer::getData() const{
    return std::string(this->data, this->pos);
}