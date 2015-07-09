#ifndef __THREAD_H__
#define __THREAD_H__

#define POSIX_THREADS

#include <unistd.h>
#include <pthread.h>

class CThread;

#include <mutex.h>
#include <mutex_lock.h>
#include <mutex_cond.h>

#ifndef CTHREAD_STACK_SIZE
#define CTHREAD_STACK_SIZE (128*1024)
#endif

class IThreadable {
protected:
	virtual int32_t run ( CThread *thread, void *info ) = 0;
	virtual ~IThreadable ( void ) {};
	friend class CThread;	
};

typedef int32_t (*cthread_func_t) ( CThread *, void *);

class CThread: public CMutexCond {
protected:
    int fthread;
    pthread_t threadId;
	IThreadable *parent;
	bool running;
	bool finish;
	void *info;
	bool sleeping;
	uint32_t stackSize;
	cthread_func_t *frun;
	
    static void *__run__( void *arg );	
	void init ( IThreadable *parent, cthread_func_t *func, void *info, bool autoStart, uint32_t stackSize );
public:
	CThread ( IThreadable *parent, void *info = NULL, bool autoStart = false, uint32_t stackSize = CTHREAD_STACK_SIZE );
	CThread ( cthread_func_t *func, void *info = NULL, bool autoStart = false, uint32_t stackSize = CTHREAD_STACK_SIZE );
	CThread ( IThreadable *parent, uint32_t stackSize );
	CThread ( cthread_func_t *func, uint32_t stackSize );
	void setInfo ( void *info );
	void *getInfo ( void ) { return info; };
	virtual ~CThread ( void );
	void start ( void );
	bool isRunning ( void ) { return running; };	
	void terminate ( void );
};
#endif
