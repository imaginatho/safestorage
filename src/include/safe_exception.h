#ifndef __SAFE_EXCEPTION__H__
#define __SAFE_EXCEPTION__H__

#include <stdint.h>
#include <string>
#include <exception>

using namespace std;

class CSafeException: exception 
{
    protected:
        int32_t _code;
        int32_t _line;
        string _file;
        string _message;
    public:
        CSafeException ( const string &sourcefile, uint32_t line, int32_t eCode ) throw();
        virtual ~CSafeException ( void ) throw();
        virtual const char *what () const throw();
        int32_t getErrorCode ( void ) const { return _code; };
        int32_t code ( void ) const { return _code; };
        int32_t line ( void ) const { return _line; };
        string file ( void ) const { return _file; };
        string msg ( void ) const { return _message; };
};

#define CSAFE_EXCEPTION_CODE(X) CSafeException(__FILE__,__LINE__, X);
#define CSAFE_EXCEPTION(X,...) 	CSafeException(__FILE__,__LINE__, X, __VA_ARGS__);

#endif

