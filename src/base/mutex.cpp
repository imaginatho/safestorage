#include <iostream>
#include <stdint.h>
#include <thread.h>
#include <mutex.h>

CMutex::CMutex ( void )
{
	pthread_mutex_init(&mutex, NULL);
}
 
CMutex::~CMutex ( void )
{
	pthread_mutex_destroy(&mutex);
}

int CMutex::lock ( const char *filename, int32_t line )
{	
	int result = pthread_mutex_lock(&mutex);
	if (!result) {
		this->filename = filename;
		this->line = line;
	}
	return result;
}

int CMutex::unlock ( void )
{
	return pthread_mutex_unlock(&mutex);
}

int CMutex::trylock ( const char *filename, int32_t line )
{
	int result = pthread_mutex_trylock(&mutex);
	if (result == 0) {
		this->filename = filename;
		this->line = line;
	}
	return result;
}

