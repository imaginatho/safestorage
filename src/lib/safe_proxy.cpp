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
#include <algorithm>
#include <string>
using namespace std;

#include <log.h>
#include <safe_storage_imp.h>
#include <safe_client.h>
#include <safe_proxy.h>

#define __RETURN_STORAGE_CALL(X) 	if (!storage) {return E_CSTORAGE_NOT_OPENED;} return storage->X

CSafeProxy::CSafeProxy ( void )
{
	storage = NULL;
	remote = false;
}

CSafeProxy::~CSafeProxy ()
{
	if (storage) delete storage;
}

int32_t CSafeProxy::close ( uint32_t flags )
{
	if (!storage)
		return E_CSTORAGE_NOT_OPENED;

	int32_t result = storage->close(flags);
	if (result == E_CSTORAGE_OK)
	{
		delete storage;
		storage = NULL;
	}
	return result;
}

int32_t CSafeProxy::open ( const string &location, uint32_t flags )
{
	string normalized;
	if (!prepareStorage(location, normalized))
		return E_CSTORAGE_ALREADY_OPENED;

	return storage->open(normalized, flags);
}

bool CSafeProxy::prepareStorage ( const string &location, string &normalized )
{
	if (storage)
		return false;

	uint32_t dotpos = location.find_last_of(':');
	bool direct = (dotpos == string::npos);

	if (!direct) {
		string aux = location;
		std::replace(aux.begin(), aux.end(), ':', ' ');
		string params = aux.substr(0, dotpos);
		printf("PARAMS = '%s'\n", params.c_str());
		// storage = new CSafeClient(105, params.c_str());
		storage = new CSafeClient();
		normalized = location.substr(dotpos+1);
	}
	else {
		storage = new CSafeStorage();
		normalized = location;
	}
	return true;
}
