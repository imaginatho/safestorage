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


CSafeStorageConnection::CSafeStorageConnection ( int fd, struct sockaddr &in_addr )
	:fd(fd)
{
}

CSafeStorageConnection::~CSafeStorageConnection ( void )
{
	if (fd >= 0) close(fd);
}

void CSafeStorageConnection::onData ( void )
{
}
