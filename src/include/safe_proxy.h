#ifndef __SAFE_PROXY__H__
#define __SAFE_PROXY__H__

#include <stdint.h>
#include <string>
#include <list>
#include <set>
using namespace std;

#include <safe_exception.h>
#include <base_safe_file.h>
#include <safe_file.h>
#include <safe_storage_bridge.h>

class CSafeProxy: public CSafeStorageBridge
{
	protected:
		bool remote;
        bool prepareStorage ( const string &location, string &normalized );

    public:
        CSafeProxy ( void );
        virtual ~CSafeProxy ();
        virtual int32_t close ( uint32_t flags = 0 );
        virtual int32_t open ( const string &location, uint32_t flags = 0 );
        virtual int32_t create ( const string &location, uint32_t flags = 0 );
};

#endif
