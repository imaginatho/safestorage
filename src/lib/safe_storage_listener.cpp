#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <syslog.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

using namespace std;

#include <safe_exception.h>
#include <safe_storage_listener.h>

CSafeStorageListener::CSafeStorageListener ( const string &params )
{
	sfd = -1;
}

CSafeStorageListener::~CSafeStorageListener ( void )
{
}
		
int32_t CSafeStorageListener::stop ( void )
{
	return E_CSTORAGE_OK;
}

int32_t CSafeStorageListener::setFdFlags ( int fd, int flags )
{
	int _flags = fcntl(fd, F_GETFL, 0);
	if (_flags < 0) return -1;
	
	_flags |= flags;
	if (fcntl(fd, F_SETFL, _flags) < 0) return -1;
	return E_CSTORAGE_OK;
}

int32_t CSafeStorageListener::openTcpPort ( int32_t port )
{
	try {
		if ((sfd = socket(AF_INET, SOCK_STREAM, 0) < 0)) 	throw CSAFE_EXCEPTION_SYS(E_CSTORAGE_OPEN_LISTEN);
		if (setFdFlags(sfd, O_NONBLOCK)) 					throw CSAFE_EXCEPTION_SYS(E_CSTORAGE_OPEN_LISTEN);

		struct sockaddr_in s_addr;
		memset(&s_addr, 0, sizeof(s_addr));
		
		s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		s_addr.sin_family= AF_INET;
		s_addr.sin_port = htons(port);

		if (bind(sfd, (struct sockaddr *) &s_addr, sizeof(s_addr)) < 0) {
			throw CSAFE_EXCEPTION_SYS(E_CSTORAGE_OPEN_LISTEN);
		}
		
		return E_CSTORAGE_OK;
	}
	catch (CSafeException &e) {
		if (sfd >= 0) close(sfd);
		sfd = -1;
		return e.code();
	}

//	if (listen(sd, SOMAXCONN))
}
