#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>

#include <iostream>
#include <sstream>
#include <string>
#include <exception>
using namespace std;

#include <log.h>
#include <shared_safe_storage.h>

#define F_SHARED_SS_WRITE_OPS (O_CSTORAGE_BRIDGE_COMMIT | O_CSTORAGE_BRIDGE_ROLLBACK | O_CSTORAGE_BRIDGE_WRITE)

COpenSafeStorages CSharedSafeStorage::storages;

CSharedSafeStorage::CSharedSafeStorage ( void )
{
	writer = false;
	storage = NULL;
}

CSharedSafeStorage::~CSharedSafeStorage ()
{
    // destructor of class CSafeStorage make rollback and close
	close(0);
}

int32_t CSharedSafeStorage::close ( uint32_t flags )
{
	if ( !storage ) {
		return E_CSTORAGE_NOT_OPENED;
	}

	int32_t result = storage->close(flags);
	
	if ( result == E_CSTORAGE_OK ) {
		lock();
		COpenSafeStorage *css = (COpenSafeStorage *)storage;
		if (css->referenceCount() == 0) {
			storages.erase(css->getRealPath());
			delete storage;
			storage = NULL;
		}
		unlock();
	}
	return result;
}

COpenSafeStorage *CSharedSafeStorage::findLocation ( const string &location )
{
	char *_rpath = realpath(location.c_str(), NULL);
	string rpath;
	if (!_rpath) rpath = location;
	else {
		rpath = _rpath;
		free(_rpath);
	}

	COpenSafeStorage *css;
	lock();
	COpenSafeStorages::iterator it = storages.find(rpath);
	if (it == storages.end()) {
		// not found any open safestorage for this location
		css = new COpenSafeStorage(rpath);		
		storages[rpath] = css;
	}
	else {
		css = it->second;
	}
	unlock();
	return css;
}
/*
int32_t CSharedSafeStorage::create ( const string &filename, uint32_t flags )
{
	int32_t result = E_CSTORAGE_ALREADY_OPENED;
	COpenSafeStorage *css = findLocation(filename);
	
	storage = css;
	if (css->referenceCount() == 0) {
		writer = true;
		css->addReference(this, writer);
		result = storage->create(filename, flags);
	}
	return result;
}
*/

int32_t CSharedSafeStorage::open ( const string &filename, uint32_t flags )
{
	int32_t result = E_CSTORAGE_ALREADY_OPENED;
	COpenSafeStorage *css = findLocation(filename);
	
	writer = flags & (F_CSFILE_WR | F_CSFILE_CREATE);
	if (writer && css->getWriter()) {
		return E_CSTORAGE_ONLY_ONE_WRITER;
	}
	
	storage = css;
	result = storage->open(filename, flags);
	css->addReference(this, writer);
	return result;
}

uint32_t CSharedSafeStorage::muteFlags ( uint32_t flags, uint32_t operation )
{
	if ( writer || operation != O_CSTORAGE_BRIDGE_READ ) return flags;
	return (flags | F_CSTORAGE_COMMITED_READ);
}

bool CSharedSafeStorage::allowed ( uint32_t operation )
{
	if ( (operation & F_SHARED_SS_WRITE_OPS) != 0 && !writer ) {
		return false;
	}
	return true;
}

void CSharedSafeStorage::lock ( void )
{		
}

void CSharedSafeStorage::unlock ( void )
{		
}
