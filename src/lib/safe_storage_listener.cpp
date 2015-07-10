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

#include <exception.h>
#include <safe_storage_listener.h>

#ifndef D_CSTORAGE_MAX_LISTENER_CONNECTIONS	
#define D_CSTORAGE_MAX_LISTENER_CONNECTIONS	32
#endif

CSafeStorageListener::CSafeStorageListener ( const string &params )
	:thread(this)
{
	sfd = efd = lastFd = -1;
	lastConnection = NULL;
	events = NULL;
	initListener(params);
	initEpoll();
}

void CSafeStorageListener::initEpoll ( void )
{
	struct epoll_event event;
	
	efd = epoll_create(D_CSTORAGE_MAX_LISTENER_CONNECTIONS);
	
	event.data.fd = sfd;
	event.events = EPOLLIN|EPOLLET;
	epoll_ctl(efd, EPOLL_CTL_ADD, sfd, &event);
	events = (struct epoll_event *)calloc(D_CSTORAGE_MAX_LISTENER_CONNECTIONS, sizeof(events[0]));	
}

void CSafeStorageListener::initListener ( const string &params )
{
	int port = atoi(params.c_str());
	openTcpPort(port);
}

int32_t CSafeStorageListener::run ( CThread *thread, void *_data )
{
	int count, index;

	while (1)
    {		
		count = epoll_wait (efd, events, D_CSTORAGE_MAX_LISTENER_CONNECTIONS, -1);
		for (index = 0; index < count; index++)
		{
			if ((events[index].events & (EPOLLERR|EPOLLHUP)) || !(events[index].events & EPOLLIN))
			{
				// An error has occured on this fd, or the socket is not ready for reading (why were we notified then?)
				close (events[index].data.fd);
				continue;
			}
	
			if (sfd == events[index].data.fd) {
				doListenEvent(events[index]);
				continue;
			}

			doDataEvent(events[index]);
		}
    }
	return 0;
}

void CSafeStorageListener::doDataEvent ( struct epoll_event &event )
{
	CSafeStorageConnection *conn = getConnection(event.data.fd);
	conn->onData();
}

void CSafeStorageListener::doListenEvent ( struct epoll_event &event )
{
	// We have a notification on the listening socket, which means one or more incoming connections.
	struct sockaddr in_addr;
	socklen_t in_len;
	int fd;

	while (1) {
		in_len = sizeof(in_addr);
		fd = accept (sfd, &in_addr, &in_len);
		if (fd == -1) break;

		// Make the incoming socket non-blocking and add it to the list of fds to monitor.
		if (setFdFlags(fd, O_NONBLOCK) < 0) {
			close(fd);
			continue;
		}

		event.data.fd = fd;
		event.events = EPOLLIN | EPOLLET;
		if (epoll_ctl (efd, EPOLL_CTL_ADD, fd, &event) < 0) close(fd);
		
		createConnection(fd, in_addr);
	}
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
		if ((sfd = socket(AF_INET, SOCK_STREAM, 0) < 0)) 	CEXP_SYS(E_CSTORAGE_OPEN_LISTEN);
		if (setFdFlags(sfd, O_NONBLOCK)) 					CEXP_SYS(E_CSTORAGE_OPEN_LISTEN);

		struct sockaddr_in s_addr;
		memset(&s_addr, 0, sizeof(s_addr));
		
		s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		s_addr.sin_family= AF_INET;
		s_addr.sin_port = htons(port);

		if (bind(sfd, (struct sockaddr *) &s_addr, sizeof(s_addr)) < 0) {
			CEXP_SYS(E_CSTORAGE_OPEN_LISTEN);
		}

		listen(sfd, SOMAXCONN);
		
		return E_CSTORAGE_OK;
	}
	catch (CException &e) {
		if (sfd >= 0) close(sfd);
		sfd = -1;
		return e.getResult();
	}

}


CSafeStorageConnection *CSafeStorageListener::getConnection ( int fd )
{
	if (lastFd == fd) return lastConnection;
	
	CSafeStorageConnections::iterator it = connections.find(fd);
	if (it == connections.end()) {
		return NULL;
	}
	
	return it->second;
}

CSafeStorageConnection *CSafeStorageListener::createConnection ( int fd, struct sockaddr &in_addr )
{
	CSafeStorageConnections::iterator it = connections.find(fd);
	if (it != connections.end()) {
		return it->second;
	}
	
	CSafeStorageConnection *conn = new CSafeStorageConnection(fd, in_addr);
	connections[fd] = conn;
	lastFd = fd;
	lastConnection = conn;
	
	return conn;
}
