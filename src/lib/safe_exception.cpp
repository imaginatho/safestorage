#include <stdarg.h>
#include <stdio.h>

#include <safe_exception.h>
#include <sstream>
#include <string>
#include <iostream>

using namespace std;

CSafeException::CSafeException ( const char *sourcefile, uint32_t line, int32_t eCode ) throw()
    :_code(eCode),_line(line),_file(sourcefile)
{
	char text[512];
	snprintf(text, sizeof(text), "CSafeException[%s:%d]:%d", sourcefile, line, eCode);
	text[sizeof(text)-1] = '\0';
    _message = text;
}

CSafeException::CSafeException ( const char *sourcefile, uint32_t line, int32_t eCode, const char *fmt, ... ) throw()
{
	va_list ap;
	char text[4096], *rest;
	char *eline = text + sizeof(text);
	
	int bytes = snprintf(text, sizeof(text), "CSafeException[%s:%d]:%d ", sourcefile, line, eCode);
	if (bytes < 0) bytes = 0;
	
	rest = text + bytes;
	va_start(ap, fmt);
	vsnprintf(rest, eline-rest, fmt, ap);
	va_end(ap);
	
	text[sizeof(text)-1] = '\0';
    _message = text;
}

CSafeException::~CSafeException ( void ) throw()
{
}

const char *CSafeException::what ( void ) const throw()
{
    return _message.c_str();
}

