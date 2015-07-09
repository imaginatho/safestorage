#ifndef __SAFE_STORAGE_LISTENER__H__
#define __SAFE_STORAGE_LISTENER__H__

#include <stdint.h>
#include <string>
#include <list>
#include <map>
#include <set>
using namespace std;

#include <safe_storage.h>

class CSafeStorageListener: public ISafeStorageListener
{
    protected:
		int sfd;
		
    public:
        CSafeStorageListener ( void );
        virtual ~CSafeStorageListener ( void );
		
		virtual int32_t stop ( void );
};

#endif