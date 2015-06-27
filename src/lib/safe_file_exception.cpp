#include <safe_file_exception.h>
#include <sstream>
#include <string>
#include <iostream>

using namespace std;

CSafeFileException::CSafeFileException ( const string &sourcefile, uint32_t line, int32_t eCode ) throw()
    :_code(eCode),_line(line),_file(sourcefile)
{
    stringstream ss;
    ss << "CSafeFileException [" << sourcefile << ":" << line << "]: " << eCode;
    _message = ss.str();
}

CSafeFileException::~CSafeFileException ( void ) throw()
{
}

const char *CSafeFileException::what ( void ) const throw()
{
    return _message.c_str();
}

