/**
 * \class COpenSafeStorage
 * 
 * \brief Class to share access to safestorage
 * 
 * Class description
 * 
 */

#ifndef __OPEN_SAFE_STORAGE__H__
#define __OPEN_SAFE_STORAGE__H__

#include <stdint.h>
#include <string>
#include <list>
#include <set>
using namespace std;

class COpenSafeStorage;

#include <base_safe_file.h>
#include <safe_file.h>
#include <safe_storage_imp.h>
#include <safe_storage_bridge.h>
#include <i_safe_reference.h>

typedef list<ISafeReference *> CSafeReferences;

class COpenSafeStorage: public CSafeStorageBridge
{
    protected:
		string realPath;
		CSafeReferences references;
		ISafeReference *writer;
    public:
		COpenSafeStorage ( const string &realPath );
		virtual ~COpenSafeStorage ( void );
        int32_t addReference ( ISafeReference *ref, bool write = false );
        int32_t removeReference ( ISafeReference *ref );
        int32_t referenceCount ( void );
		void lock ( void );
		void unlock ( void );
		ISafeReference *getWriter ( void ) { return writer; };
		const string &getRealPath ( void ) { return realPath; };
};

#endif
