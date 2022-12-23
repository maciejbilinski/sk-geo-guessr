#ifndef __IncorrectPort__
#define __IncorrectPort__
#include <exception>

class IncorrectPort : public std::exception{
    virtual const char* what() const throw(){
        return "Port in not a number or is out of range (1-65535)";
    }
};


#endif // __IncorrectPort__
