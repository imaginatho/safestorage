#ifndef __C_LOG__H__
#define __C_LOG__H__

#include <stdint.h>
#include <string.h>

#ifndef C_LOG_LEVEL
#define C_LOG_LEVEL 8
#endif

#ifndef C_LOG_RUN_LEVEL
#define C_LOG_RUN_LEVEL 0
#endif

extern int32_t c_log_runtime_level;

void c_log_message ( int8_t level, const char *filename, int32_t line, const char *funcname, const char *fullfuncname,
					const char *format, ... );

char *c_log_data2hex ( void *data, int32_t offset, int32_t len, int32_t null_at_end = 1 );

int32_t c_log_set_level ( int32_t level );
int32_t c_log_get_level ( void );

const char *strbacktrace ( int32_t levels, char *data = NULL, int dlen = 0);

#define C_LOG_MESSAGE(LEVEL,...)     {if (c_log_runtime_level > LEVEL) c_log_message(LEVEL, __FILE__, __LINE__, __FUNCTION__, __PRETTY_FUNCTION__, __VA_ARGS__);}

#if(C_LOG_LEVEL>0)
#	define C_LOG_ERR(...) C_LOG_MESSAGE(0,__VA_ARGS__)
#	define C_LOG_ERR_ENABLED (c_log_runtime_level >= 0)	
#else
#	define C_LOG_ERR(...)
#	define C_LOG_ERR_ENABLED 0
#endif

#if(C_LOG_LEVEL>1)
#	define C_LOG_WARN(...) C_LOG_MESSAGE(1,__VA_ARGS__)
#	define C_LOG_WARN_ENABLED (c_log_runtime_level >= 1)	
#else
#	define C_LOG_WARN(...)
#	define C_LOG_WARN_ENABLED 0
#endif

#if(C_LOG_LEVEL>2)
#	define C_LOG_INFO(...) C_LOG_MESSAGE(2,__VA_ARGS__)
#	define C_LOG_INFO_ENABLED (c_log_runtime_level >= 2)	
#else
#	define C_LOG_INFO(...)
#	define C_LOG_INFO_ENABLED 0
#endif

#if(C_LOG_LEVEL>3)
#	define C_LOG_TRACE(...) C_LOG_MESSAGE(3,__VA_ARGS__)
#	define C_LOG_TRACE_ENABLED (c_log_runtime_level >= 3)	
#else
#	define C_LOG_TRACE(...)
#	define C_LOG_TRACE_ENABLED 0
#endif

#if(C_LOG_LEVEL>4)
#	define C_LOG_DEBUG(...) C_LOG_MESSAGE(4,__VA_ARGS__)
#	define C_LOG_DEBUG_ENABLED (c_log_runtime_level >= 4)	
#else
#	define C_LOG_DEBUG(...)
#	define C_LOG_DEBUG_ENABLED 0
#endif

#endif
