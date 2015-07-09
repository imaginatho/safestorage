#ifndef __MUTEX_COND_H__
#define __MUTEX_COND_H__

#define POSIX_THREADS

#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <map>

class CMutexCond;

#include <mutex.h>
#include <mutex_lock.h>

#define CMUTEX_COND_F_GETUS			0x0001
#define CMUTEX_COND_F_DONT_UNLOCK	0x0002
#define CMUTEX_COND_F_DONT_LOCK		0x0004
#define CMUTEX_COND_F_ONLY_WAIT		(CMUTEX_COND_F_DONT_UNLOCK | CMUTEX_COND_F_DONT_UNLOCK)

class CMutexCond: public CMutex {
	protected:
		struct timeval lstp,letp;
		pthread_cond_t cond;
		uint32_t lastRealSleepTime;
		bool sleeping;
	public:
		CMutexCond ( void );
		virtual ~CMutexCond ( void );
		uint32_t sleep ( int32_t ms = -1, const char *filename = NULL, int32_t line = 0, uint32_t flags = 0 );
		uint32_t waitLock ( int32_t ms = -1, const char *filename = NULL, int32_t line = 0, uint32_t flags = 0 ) { return sleep(ms, filename, line, flags | CMUTEX_COND_F_DONT_UNLOCK); };
		uint32_t wait ( int32_t ms = -1, const char *filename = NULL, int32_t line = 0, uint32_t flags = 0 ) { return sleep(ms, filename, line, flags | CMUTEX_COND_F_ONLY_WAIT); };
		void wakeUp ( const char *filename = NULL, int32_t line = 0, uint32_t flags = 0 );
		void wakeUpUnlock ( const char *filename = NULL, int32_t line = 0, uint32_t flags = 0 ) { wakeUp(filename, line, flags | CMUTEX_COND_F_DONT_LOCK); };
		bool isSleeping ( void ) { return sleeping; };
		uint64_t getUsLastSleepStartTime ( void ) { return (lstp.tv_sec * 1000000ULL + lstp.tv_usec); };
		uint64_t getUsLastSleepEndTime ( void ) { return (letp.tv_sec * 1000000ULL + letp.tv_usec); };
		uint32_t getUsLastSleepTime ( void ) { return (letp.tv_sec == 0 || lstp.tv_sec == 0) ? 0 : (uint32_t)((int64_t)(letp.tv_sec-lstp.tv_sec) * 1000000ULL + (int64_t)(letp.tv_usec-lstp.tv_usec)); };
};

template <typename K, typename V> 
class CMutexConMap: public CMutex, public std::map<K,V>
{
};
#endif
