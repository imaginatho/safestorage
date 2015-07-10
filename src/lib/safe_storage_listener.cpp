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
	sfd = -1;
	events = NULL;
	initListener();
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

void CSafeStorageListener::initListener ( void )
{
	
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
}

void CSafeStorageListener::doDataEvent ( struct epoll_event &event )
{
/*	
	// We have data on the fd waiting to be read. Read and display it. We must read whatever data is available
	// completely, as we are running in edge-triggered mode and won't get a notification again for the same data.
	int done = 0;

	while (1)
	{
		ssize_t count;
		char buf[512];

		count = read (events[i].data.fd, buf, sizeof buf);
		if (count == -1)
		{
			// If errno == EAGAIN, that means we have read all data. So go back to the main loop.
			if (errno != EAGAIN)
			{
				perror ("read");
				done = 1;
			}
			break;
		}
		else if (count == 0)
		{
			// End of file. The remote has closed the connection.
			done = 1;
			break;
		}

		// Write the buffer to standard output
		s = write (1, buf, count);
		if (s == -1)
		{
			perror ("write");
			abort ();
		}
	}

	if (done)
	{
		printf ("Closed connection on descriptor %d\n",
		events[i].data.fd);

		// Closing the descriptor will make epoll remove itfrom the set of descriptors which are monitored.
		close (events[i].data.fd);
	}*/
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
