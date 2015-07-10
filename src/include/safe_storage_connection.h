#ifndef __SAFE_STORAGE_CONNECTION__H__
#define __SAFE_STORAGE_CONNECTION__H__

#include <stdint.h>
#include <string>
#include <list>
#include <map>
#include <set>
using namespace std;

#include <thread.h>
#include <safe_storage.h>


class CSafeStorageConnection
{
    protected:
		int fd;
		uint8_t *dbegin;
		uint32_t dsize;
		uint8_t *dend;
		uint8_t *data;
				
    public:
        CSafeStorageConnection ( int fd, struct sockaddr &in_addr );
        virtual ~CSafeStorageConnection ( void );
		
		virtual void onData ( void );
};

#endif