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

    public:
        CSafeStorageListener ( void );
        virtual ~CSafeStorageListener ();
		
		virtual int32_t stop ( void );
};

#endif