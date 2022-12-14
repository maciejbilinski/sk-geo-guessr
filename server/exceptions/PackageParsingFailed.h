#ifndef __PackageParsingFailed__
#define __PackageParsingFailed__
#include <exception>

class PackageParsingFailed : public std::exception{
    virtual const char* what() const throw(){
        return "Package parser failed!";
    }
};


#endif 
