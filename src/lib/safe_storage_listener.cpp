#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <syslog.h>

using namespace std;

#include <safe_storage_listener.h>

CSafeStorageListener::CSafeStorageListener ( void )
{
}

CSafeStorageListener::~CSafeStorageListener ( void )
{
}
		
int32_t CSafeStorageListener::stop ( void )
{
	return E_CSTORAGE_OK;
}