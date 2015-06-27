#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdarg.h>
#include <errno.h>
#include <stdlib.h>
#include <execinfo.h>

using namespace std;

#include <c_log.h>
#include <list>


static list<char *> tmp_data;
int32_t c_log_runtime_level = C_LOG_RUN_LEVEL;

const char *strbacktrace ( int32_t levels, char *data, int dlen )
{
    static char tdata[1024];
    void *array[64];
    size_t size;
    char **strings;
    size_t i;

     if (!data) {
             data = tdata;
             dlen = sizeof(tdata)-1;
     }
     --dlen;

    size = backtrace (array, levels > 64 ? 64 : levels);
    strings = backtrace_symbols (array, size);
     char *pos, *aux, *aux2, *ppos = NULL;
    for (i = 0; i < size; i++) {
             if (i) strncat(data, " ", dlen);
             pos = strchr(strings[i], '[');
             if (!pos) {
                     aux2 = strchr(strings[i], '(');
                     if (aux2) *aux2 = '\0';
                     strncat(data, strings[i], dlen);
             }
             else {
                     *pos = '\0';
                     ++pos;
                     aux = strchr(pos, ']');
                     aux2 = strchr(strings[i], '(');
                     if (aux2) *aux2 = '\0';
                     if (aux) *aux = '\0';
                     if (!ppos || strcmp(strings[i], strings[i-1])) {
                             strncat(data, strings[i], dlen);
                             strncat(data, " ", dlen);
                     }
                     strncat(data, pos, dlen);
             }
             ppos = pos;
     }

    free (strings);
     return data;
}


int32_t c_log_set_level ( int32_t level )
{
	if (level < 0)
		level = 0;
	if (level > C_LOG_LEVEL)
		level = C_LOG_LEVEL;

	printf("c_log_set_level(%d)\n", level);
	c_log_runtime_level = level;
	return level;
}

int32_t c_log_get_level ( void )
{
	printf("c_log_get_level()=%d\n", c_log_runtime_level);
	return c_log_runtime_level;
}
/*
 * This function clears all temporal data generated with function c_log_data2hex
 */

void c_log_clear ( void )
{
	list<char *>::iterator it = tmp_data.begin();

	while (it != tmp_data.end())
	{
		printf("c_log_clear - free(%p)\n", *it);
		free(*it);
		++it;
	}
	tmp_data.clear();
}

void __c_log_message ( int8_t level, const char *filename, int32_t line, const char *funcname, const char *fullfuncname,
					const char *format, va_list args )
{
	static int32_t colors[] = {31, 33, 34, 35, 32};
/*	if (strncmp(filename, "./", 2) == 0)
		filename += 2;*/
	const char *p_slash = strrchr(filename, '/');
	if (p_slash) {
		filename = p_slash+1;
	}
	printf("\x1B[%dm[%d %s:%d %s] ", colors[level<=4?(level<0?0:level):4], level, filename, line, funcname);
	vprintf(format, args);
	printf("\x1B[0m\n");
	c_log_clear();
}

/*
 * Encapsulate error logs output.
 *
 * @param level indicates critical of error, from more critical (0) to less critical (4).
 * @param filename source where this error was produced.
 * @param line line of source where this error was produced.
 * @param funcname function where this error was produced.
 * @param fullfuncname fuction with arguments where this error was produced.
 * @param format format of message, style printf.
 */

void c_log_message ( int8_t level, const char *filename, int32_t line, const char *funcname, const char *fullfuncname,
					const char *format, ... )
{
	va_list args;
    va_start(args, format);
    __c_log_message(level, filename, line, funcname, fullfuncname, format, args);
    va_end(args);
}

/*
 * Generates a temporal auto-free hex data from binary data
 *
 * @param data pointer to binary data
 * @param offset bytes to skip
 * @param len length in bytes of data converted in hex format.
 */

char *c_log_data2hex ( void *data, int32_t offset, int32_t len, int32_t null_at_end )
{
	// static string to fast convert from int to hexa
	static char *hex = "0123456789ABCDEF";

	// reserve space for hex data
	char *hdata = (char *)malloc(len*2+1);
	printf("c_log - malloc(%d) = %p\n", len*2+1, hdata);
	// add this data in list to free after clog_message
	tmp_data.push_back(hdata);

	// initialize temporal pointers
	int8_t *cdata = (int8_t *)data;
	cdata += offset;
	char *chdata = hdata;

	// loop to convert one byte binary in two bytes hex
	while (len)
	{
		*chdata = hex[(*cdata >> 4) & 0x0F];
		++chdata;
		*chdata = hex[*cdata & 0x0F];
		++chdata;

		++cdata;
		--len;
	}
	if (null_at_end) {
		*chdata = '\0';
	}
	return hdata;
}

