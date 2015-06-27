#ifndef __SAFE_FILE_EXCEPTION__H__
#define __SAFE_FILE_EXCEPTION__H__

#include <stdint.h>
#include <string>
#include <exception>

using namespace std;

class CSafeFileException: exception 
{
    protected:
        int32_t _code;
        int32_t _line;
        string _file;
        string _message;
    public:
        CSafeFileException ( const string &sourcefile, uint32_t line, int32_t eCode ) throw();
        virtual ~CSafeFileException ( void ) throw();
        virtual const char *what () const throw();
        int32_t getErrorCode ( void ) const { return _code; };
        int32_t code ( void ) const { return _code; };
        int32_t line ( void ) const { return _line; };
        string file ( void ) const { return _file; };
        string msg ( void ) const { return _message; };
};

#endif

