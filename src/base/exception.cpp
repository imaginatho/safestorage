#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <exception.h>

CException::CException ( const char *file, uint32_t line ) throw()
{
    this->message = "";
	this->message_pos = 0;
    this->file = file;
    this->line = line;
    this->result = 0;
}

CException::CException ( const char *file, uint32_t line, const char *format, ... ) throw()
{
    va_list va_args;
    va_start(va_args, format);
	init("[%s:%d] ", file, line, NULL, format, va_args);
    va_end(va_args);
}

CException::CException ( const char *file, uint32_t line, int32_t result, const char *format, ... ) throw()
{
    va_list va_args;
    va_start(va_args, format);
	init("[E:%d %s:%d] ", file, line, &result, format, va_args);
    va_end(va_args);
}

CException::CException ( const char *file, uint32_t line, int32_t result ) throw()
{
    init("[E:%d %s:%d]", file, line, &result, NULL, NULL);
}

void CException::init ( int32_t *result, const char *format, ... )  throw()
{
    va_list va_args;
    va_start(va_args, format);
	init("[E:%d %s:%d] ", NULL, line, result, format, va_args);
    va_end(va_args);
}

void CException::init ( const char *ref_format, const char *file, uint32_t line, int32_t *result, const char *format, va_list va_args ) throw()
{
    char *tmp;
    
    if (file) {
		this->file = file;
		this->line = line;	
	}
    int32_t len = format ? vsnprintf(NULL, 0, format, va_args) : 0;
	int32_t len_ref;
	if (result) len_ref = snprintf(NULL, 0, ref_format, (*result), this->file.c_str(), this->line);
	else len_ref = snprintf(NULL, 0, ref_format, this->file.c_str(), this->line);
		
	int32_t size = len+len_ref+16;

    tmp = (char *)malloc(size);
	if (result) len_ref = snprintf(tmp, size, ref_format, (*result), this->file.c_str(), this->line);
	else len_ref = snprintf(tmp, size, ref_format, this->file.c_str(), this->line);
	if (format) {
		size -= len_ref;
		vsnprintf(tmp + len_ref, size, format, va_args );
	}
    this->message = tmp;
	this->message_pos = len_ref;
    free(tmp);
    this->result = result ? *result : 0;
}

CException::~CException( void ) throw()
{
}

const char *CException::getMessage ( void ) const
{ 
    if (message_pos > 0 && message_pos < (int32_t)strlen(message.c_str())) {
        return (message.c_str() + message_pos);
    }
    return message.c_str();
}

const char *CException::what ( void ) const throw()
{
    return message.c_str();
}

const char *CException::getFile ( void ) const
{
    return file.c_str();
}

int32_t CException::getLine ( void ) const
{
    return line;
}

int32_t CException::getResult ( void ) const
{
    return result;
}
