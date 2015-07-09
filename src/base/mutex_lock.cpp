#include <iostream>
#include <stdint.h>
#include <mutex_lock.h>

CMutexLock::CMutexLock ( CMutex *mutex, const char *filename, int32_t line, bool onlyTry )
	:mutex(mutex)
{
	init(filename, line, true, onlyTry);
}

CMutexLock::CMutexLock ( CMutex &mutex, const char *filename, int32_t line, bool onlyTry )
	:mutex(&mutex)
{
	init(filename, line, true, onlyTry);
}

CMutexLock::CMutexLock ( CMutex *mutex, bool lockCondition, const char *filename, int32_t line, bool onlyTry )
	:mutex(mutex)
{
	init(filename, line, lockCondition, onlyTry);
}

CMutexLock::CMutexLock ( CMutex &mutex, bool lockCondition, const char *filename, int32_t line, bool onlyTry )
	:mutex(&mutex)
{
	init(filename, line, lockCondition, onlyTry);
}

void CMutexLock::init ( const char *filename, int32_t line, bool lockCondition, bool onlyTry )
{
	flocked = false;
	if (!lockCondition) return;
	
	if (onlyTry) {
		if (mutex->trylock(filename, line) == 0) {
			flocked = true;
		}
	}
	else {
		mutex->lock(filename, line);
		flocked = true;
	}
}
 
CMutexLock::~CMutexLock ( void )
{
	if (flocked) {
		mutex->unlock();
	}
}
