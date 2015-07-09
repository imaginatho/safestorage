#ifndef __MUTEX_LOCK_H__
#define __MUTEX_LOCK_H__

class CMutexLock;

#include <mutex.h>

#include <exception.h>

#define CMUTEX_LOCK_OR_RETURN(m,r) CMutexLock CEXP_MACRO_CONCAT(__mutexLock_,__LINE__)(m,__FILE__,__LINE__,true); if(!CEXP_MACRO_CONCAT(__mutexLock_,__LINE__).locked()) return r;
#define CMUTEX_LOCK(m) CMutexLock CEXP_MACRO_CONCAT(__mutexLock_,__LINE__)(m,__FILE__,__LINE__);
#define CMUTEX_TRYLOCK(m) CMutexLock CEXP_MACRO_CONCAT(__mutexLock_,__LINE__)(m,__FILE__,__LINE__, true);

class CMutexLock {
	protected:
		CMutex *mutex;
		bool flocked;
		
		void init ( const char *filename , int32_t line , bool lockCondition, bool onlyTry );
	public:
		CMutexLock ( CMutex *mutex, const char *filename = NULL, int32_t line = 0, bool onlyTry = false );
		CMutexLock ( CMutex &mutex, const char *filename = NULL, int32_t line = 0, bool onlyTry = false );
		CMutexLock ( CMutex *mutex, bool lockCondition, const char *filename = NULL, int32_t line = 0, bool onlyTry = false );
		CMutexLock ( CMutex &mutex, bool lockCondition, const char *filename = NULL, int32_t line = 0, bool onlyTry = false );
		virtual ~CMutexLock ( void );
		bool locked ( void ) const { return flocked; };
};

template <typename K> 
class CBlockValue {
	K *_ref;
	K _outvalue;
public:
	CBlockValue(K &ref, K invalue, K outvalue);
	virtual ~CBlockValue ( void );
};

template <typename K> 
CBlockValue<K>::CBlockValue ( K &ref, K invalue, K outvalue )
{
	_ref = &ref;
	*_ref = invalue;
	_outvalue = outvalue;
}

template <typename K> 
CBlockValue<K>::~CBlockValue ( void )
{
	*_ref = _outvalue;
}

#define CMUTEX_BLOCK_ONCE_R(v,r) if(v) return r; CBlockValue<bool> CEXP_MACRO_CONCAT(__blockvalue__, __LINE__)(v,true,false); 
#define CMUTEX_BLOCK_ONCE(v) if(v) return; CBlockValue<bool> CEXP_MACRO_CONCAT(__blockvalue__, __LINE__)(v,true,false);

#endif
