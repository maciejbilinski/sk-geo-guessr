#ifndef __IncorrectArguments__
#define __IncorrectArguments__
#include <exception>

class IncorrectArguments : public std::exception{
    virtual const char* what() const throw(){
        return "You need to pass exactly one argument representing PORT";
    }
};


#endif // __IncorrectArguments__
