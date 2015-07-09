#ifndef __SAFE_STORAGE_LISTENER__H__
#define __SAFE_STORAGE_LISTENER__H__

#include <stdint.h>
#include <string>
#include <list>
#include <map>
#include <set>
using namespace std;

#include <safe_storage.h>

#define E_CSTORAGE_OPEN_LISTEN		-1100

class CSafeStorageListener: public ISafeStorageListener
{
    protected:
		int sfd;
		static int32_t setFdFlags ( int fd, int flags );
		int32_t openTcpPort ( int32_t port );
    public:
        CSafeStorageListener ( const string &params );
        virtual ~CSafeStorageListener ( void );
		
		virtual int32_t stop ( void );
};

#endif