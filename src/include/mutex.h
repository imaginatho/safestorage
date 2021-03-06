#ifndef __MUTEX_H__
#define __MUTEX_H__

#define POSIX_THREADS

#include <unistd.h>
#include <pthread.h>
#include <map>

class CMutex;

#include <mutex_lock.h>

class CMutex {
protected:
	const char *filename;
	int32_t line;
	pthread_mutex_t mutex;
	
public:
	CMutex ( void );
	virtual ~CMutex ( void );

	int lock ( void ) { return pthread_mutex_lock(&mutex); };
	int unlock ( void ) { return pthread_mutex_unlock(&mutex); };

	int lock ( const char *filename, int32_t line );
	int trylock ( const char *filename = NULL, int32_t line = 0 );
};

template <typename K, typename V> 
class CMutexMap: public CMutex, public std::map<K,V>
{
};
#endif
