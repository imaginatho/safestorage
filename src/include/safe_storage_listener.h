#ifndef __SAFE_STORAGE_LISTENER__H__
#define __SAFE_STORAGE_LISTENER__H__

#include <stdint.h>
#include <string>
#include <list>
#include <map>
#include <set>
using namespace std;

#include <thread.h>
#include <safe_storage.h>
#include <safe_storage_connection.h>

#define E_CSTORAGE_OPEN_LISTEN		-1100

typedef map <int, CSafeStorageConnection *> CSafeStorageConnections;

class CSafeStorageListener: public ISafeStorageListener, IThreadable
{
    protected:
		CThread thread;
		CSafeStorageConnections connections;
		CSafeStorageConnection *lastConnection;	
		int sfd, efd, lastFd;
		struct epoll_event *events;
				
		static int32_t setFdFlags ( int fd, int flags );
		int32_t openTcpPort ( int32_t port );
		void initEpoll ( void );
		void initListener ( const string &params );
		virtual int32_t run ( CThread *thread, void *_data );
		void doListenEvent ( struct epoll_event &event );
		void doDataEvent ( struct epoll_event &event );
		CSafeStorageConnection *getConnection ( int fd );
		CSafeStorageConnection *createConnection ( int fd, struct sockaddr &in_addr );
		
    public:
        CSafeStorageListener ( const string &params );
        virtual ~CSafeStorageListener ( void );
		
		virtual int32_t stop ( void );
};

#endif