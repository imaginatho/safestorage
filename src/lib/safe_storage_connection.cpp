#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

using namespace std;

#include <exception.h>
#include <safe_storage_connection.h>
#include <safe_storage_listener.h>

#define BLOCK_SIZE 	(4*1024)

CSafeStorageConnection::CSafeStorageConnection ( int fd, struct sockaddr &in_addr )
	:fd(fd)
{
	dsize = 128 * 1024;
	dbegin = (uint8_t *)malloc(dsize);
	dend = dbegin + dsize;
	dcur = dbegin;
}

CSafeStorageConnection::~CSafeStorageConnection ( void )
{
	if (fd >= 0) close(fd);
}

void CSafeStorageConnection::realloc ( uint32_t size )
{
	if (dsize >= size) return;
	dsize = ((size / BLOCK_SIZE) + (size % BLOCK_SIZE ? 1: 0)) * BLOCK_SIZE;
	
	uint32_t doffset = dcur ? dcur - dbegin : 0;
	dbegin = (uint8_t *)::realloc(dbegin, dsize);
	dend = dbegin + dsize;
	if (dcur) dcur = dbegin + doffset;
}

void CSafeStorageConnection::onData ( void )
{
}

#undef BLOCK_SIZE