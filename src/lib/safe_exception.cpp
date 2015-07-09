#include <safe_exception.h>
#include <sstream>
#include <string>
#include <iostream>

using namespace std;

CSafeException::CSafeException ( const string &sourcefile, uint32_t line, int32_t eCode ) throw()
    :_code(eCode),_line(line),_file(sourcefile)
{
    stringstream ss;
    ss << "CSafeException [" << sourcefile << ":" << line << "]: " << eCode;
    _message = ss.str();
}

CSafeException::~CSafeException ( void ) throw()
{
}

const char *CSafeException::what ( void ) const throw()
{
    return _message.c_str();
}

