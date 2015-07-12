#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include <sys/time.h>

#include <mutex.h>
#include <mutex_lock.h>

uint32_t getus ( void )
{
    static uint64_t refus = 0;
    struct timeval tv;
	
    gettimeofday(&tv, NULL);
    uint64_t us = ((uint64_t)tv.tv_sec)*1000000;
    us += tv.tv_usec;
	if (refus == 0) {
		refus = us -1;
		return 1;
	}
	
	return (us - refus);
}

int32_t counter;

CMutex mt;

int f_wo_mutex ( void )
{
	++counter;
}

int f_mutex ( void )
{
	mt.lock();
	++counter;
	mt.unlock();
}

int f_mutex_c ( void )
{
	CMutexLock l(mt);
	++counter;
}

#define LOOPS	10000

int main ( int argc, char *argv[] )
{
	uint32_t us1, us2, us3, us4;
	us1 = getus();
	for (int index = 0; index < LOOPS; ++index) {
		f_wo_mutex();
	}
	us2 = getus();
	for (int index = 0; index < LOOPS; ++index) {
		f_mutex();
	}
	us3 = getus();
	for (int index = 0; index < LOOPS; ++index) {
		f_mutex_c();
	}
	us4 = getus();
	printf("f_wo_mutex: %.3f ms\n", (us2-us1)/1000.0);
	printf("f_mutex: %.3f ms\n", (us3-us2)/1000.0);
	printf("f_mutex_c: %.3f ms\n", (us4-us3)/1000.0);
}