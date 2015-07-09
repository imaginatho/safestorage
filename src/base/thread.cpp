#include <sys/time.h>
#include <iostream>
#include <stdint.h>
#include <pthread.h>
#include <errno.h>
#include <thread.h>
#include <exception.h>

#ifndef MICROSECONDS_BY_SECOND
#define MICROSECONDS_BY_SECOND	(1000*1000)
#endif

CThread::CThread ( IThreadable *parent, void *info, bool autoStart, uint32_t stackSize )
{	
	init(parent, NULL, info, autoStart, stackSize);
}

CThread::CThread ( cthread_func_t *func, void *info, bool autoStart, uint32_t stackSize )
{	
	init(NULL, func, info, autoStart, stackSize);
}

CThread::CThread ( IThreadable *parent, uint32_t stackSize )
{	
	init(parent, NULL, NULL, false, stackSize);
}

CThread::CThread ( cthread_func_t *func, uint32_t stackSize )
{	
	init(NULL, func, NULL, false, stackSize);
}

void CThread::init ( IThreadable *parent, cthread_func_t *func, void *info, bool autoStart, uint32_t stackSize )
{	
	this->parent = parent;
	this->stackSize = stackSize;
	finish = false;
	frun = func;
	running = false;
	if (autoStart) start();
}

CThread::~CThread ( void )
{
	finish = true;
	pthread_cancel(threadId);
}

void CThread::terminate ( void )
{
	finish = true;
}

void CThread::setInfo ( void *info )
{
	this->info = info;
}

void CThread::start ( void )
{
	if (!running) {
		running = true;
		pthread_attr_t attr;
	
		pthread_attr_init(&attr);
		pthread_attr_setstacksize(&attr, stackSize);
		fthread = pthread_create(&threadId, &attr, &__run__, (void*)this);
	}
}

void *CThread::__run__ ( void *arg )
{
	int32_t ms;
	CThread *_this = (CThread*)arg;
	while (!_this->finish) {
		if (_this->parent) ms = _this->parent->run(_this, _this->info);
		else ms = (*_this->frun)(_this, _this->info);
		if (ms < 0 || _this->finish) break;
		if (ms == 0) {
			sched_yield();
		}
		else {
			_this->sleep(ms);
		}
	}
	return _this;
}
