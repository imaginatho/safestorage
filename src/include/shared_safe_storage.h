#ifndef __SHARED_SAFE_STORAGE__H__
#define __SHARED_SAFE_STORAGE__H__

#include <stdint.h>
#include <string>
#include <list>
#include <set>
using namespace std;

class CSharedSafeStorage;

#include <base_safe_file.h>
#include <safe_file.h>
#include <safe_storage_imp.h>
#include <i_safe_reference.h>
#include <safe_storage_bridge.h>
#include <open_safe_storage.h>

typedef map<string, COpenSafeStorage *> COpenSafeStorages;

class CSharedSafeStorage: public ISafeReference, public CSafeStorageBridge
{
    protected:
		bool writer;
		static COpenSafeStorages storages;
		virtual uint32_t muteFlags ( uint32_t flags, uint32_t operation );
		virtual bool allowed ( uint32_t operation );		
		static void lock ( void );
		static void unlock ( void );
    public:
		CSharedSafeStorage ( void );
		virtual ~CSharedSafeStorage ( void );
		virtual void onSharedClose ( void ) {};	
        virtual int32_t close ( uint32_t flags = 0 );
        virtual int32_t open ( const string &filename, uint32_t flags = 0 );
		static COpenSafeStorage *findLocation ( const string &location );
};

#endif
