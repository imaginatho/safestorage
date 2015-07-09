#include <iostream>
#include <stdint.h>
#include <thread.h>
#include <errno.h>
#include <mutex_cond.h>

CMutexCond::CMutexCond ( void )	
{
	sleeping = false;
	lastRealSleepTime = 0;
	pthread_cond_init(&cond, NULL);
}

CMutexCond::~CMutexCond ( void )
{
	pthread_cond_destroy(&cond);
}

uint32_t CMutexCond::sleep ( int32_t ms, const char *filename, int32_t line, uint32_t flags )
{
	struct timespec ts;
	struct timeval tp;

	lstp.tv_sec = lstp.tv_usec = letp.tv_sec = letp.tv_usec = 0;
	
	// verifica si el tiempo de espera es cero, no debe esperar, en tal caso retorna
	if ( ms == 0 ) return 0;

	if (flags & CMUTEX_COND_F_GETUS) {
		gettimeofday(&lstp, NULL);
	}
	
	if ((flags & CMUTEX_COND_F_DONT_LOCK) == 0) {
		// bloqueo del acceso a la cond
		CEXP_ASSERT_RESULT(lock(filename ? filename : __FILE__, line ? line : __LINE__), "ERROR %d on CMutexCond::sleep(%d) lock", result, ms);
	}
	
	if ( ms > 0 ) {
		// en caso de especificar los tiempos realiza los calculos que finalmente se expresaran en nanosegundos
		// dentro de la estructura tp (tiempo absoluto).
		CEXP_ASSERT_RESULT(gettimeofday(&tp, NULL), "ERROR %d on CMutexCond::sleep(%d) gettimeofday", result, ms);
		tp.tv_sec += (ms / 1000);
		
		// me quedo solo con los milisegundos, sin segundos
		int32_t rms = ms % 1000;
		
		
		tp.tv_usec += (rms * 1000);
		
		// en caso de superar los microsegundos que tiene un segundo tenemos que reajustar, pasando parte de estos
		// microsegundos a segundos, para asi dejar los valores normalizados.
		if (tp.tv_usec >= 1000000) {
			tp.tv_sec += (tp.tv_usec / 1000000);
			tp.tv_usec = (tp.tv_usec % 1000000);
		}
		ts.tv_sec  = tp.tv_sec;
		ts.tv_nsec = tp.tv_usec * 1000;
	}
	
	// segun esten especificado el tiempo o este sea negativo, se llama a la espera con o sin timeout.
	sleeping = true;
	int32_t result;
	if ( ms > 0 ) {
		result = pthread_cond_timedwait(&cond, &mutex, &ts);
		CEXP_ASSERT(result == 0 || result == ETIMEDOUT, "ERROR %d on CMutexCond::sleep(%d) pthread_cond_timedwait", result, ms);
	}
	else {
		result = pthread_cond_wait(&cond, &mutex);
		CEXP_ASSERT(result == 0, "ERROR %d on CMutexCond::sleep(%d) pthread_cond_wait", result, ms);
	}
	
	sleeping = false;
	
	if ((flags & CMUTEX_COND_F_DONT_UNLOCK) == 0) {
		// debloqueo del mutex
		CEXP_ASSERT_RESULT(unlock(),  "ERROR %d on CMutexCond::sleep(%d) unlock", result, ms); 
	}
	
	if (flags & CMUTEX_COND_F_GETUS) {
		gettimeofday(&letp, NULL);
	}		
	return result;
}

void CMutexCond::wakeUp ( const char *filename, int32_t line, uint32_t flags )
{
	if ((flags & CMUTEX_COND_F_DONT_LOCK) == 0) {
		CEXP_ASSERT_RESULT(lock(filename ? filename : __FILE__, line ? line : __LINE__), "ERROR %d on CMutexCond::wakeUp() lock", result);
	}
	CEXP_ASSERT_RESULT(pthread_cond_broadcast(&cond), "ERROR %d on CMutexCond::wakeUp() pthread_cond_broadcast", result);
	if ((flags & CMUTEX_COND_F_DONT_UNLOCK) == 0) {
		CEXP_ASSERT_RESULT(unlock(), "ERROR %d on CMutexCond::wakeUp() unlock", result);
	}
}

