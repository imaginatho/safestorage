#ifndef __SAFE_STORAGE_REPLICA__H__
#define __SAFE_STORAGE_REPLICA__H__

#include <stdint.h>
#include <string>
#include <list>
#include <map>
#include <set>
using namespace std;

#include <safe_storage.h>

class CSafeStorageReplica: public ISafeStorageReplica
{
    protected:

    public:
        CSafeStorageReplica ( void );
        virtual ~CSafeStorageReplica ();
		
		virtual int32_t stop ( void );
};

#endif