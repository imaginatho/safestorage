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
#include <open_safe_storage.h>

COpenSafeStorage::COpenSafeStorage ( const string &realPath )
	:realPath(realPath)
{
	storage = new CSafeStorage();
	writer = NULL;
}

COpenSafeStorage::~COpenSafeStorage ()
{
	CSafeReferences::iterator it = references.begin();

    while (it != references.end()) {
    	(*it)->onSharedClose();
    	++it;
    }
	if (storage) delete storage;
}

int32_t COpenSafeStorage::addReference ( ISafeReference *ref, bool write )
{
	if (write) {
		if (writer) {
			return E_CSTORAGE_ONLT_ONE_WRITTER;
		}
		writer = ref;
	}

	references.push_back(ref);
	return E_CSTORAGE_OK;
}

int32_t COpenSafeStorage::removeReference ( ISafeReference *ref )
{
	if (writer == ref) {
		writer = NULL;
		if (storage) storage->rollback();
	}
	references.remove(ref);
	return E_CSTORAGE_OK;
}

int32_t COpenSafeStorage::referenceCount ( void )
{
	return references.size();
}

void COpenSafeStorage::lock ( void )
{
	// TODO
}

void COpenSafeStorage::unlock ( void )
{
	// TODO
}
