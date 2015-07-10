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
#include <safe_net.h>

class CSafeStorageConnection
{
    protected:
		int fd;
		uint8_t *dbegin;
		uint32_t dsize;
		uint8_t *dend;
		uint8_t *dcur;
			
		void realloc ( uint32_t size );
    public:
        CSafeStorageConnection ( int fd, struct sockaddr &in_addr );
        virtual ~CSafeStorageConnection ( void );
		
		int32_t onData ( void );
		int32_t onRequest ( CSAFE_NET_HEADER *hdr, uint8_t *data, uint32_t dlen );
};

#endif