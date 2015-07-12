#ifndef __SAVE_CLIENT_H__
#define __SAVE_CLIENT_H__

class CSafeClient;

#include <safe_storage.h>
#include <map>
#include <string>

using namespace std;

class CSafeClient: public ISafeStorage
{
    protected:
		uint8_t *dbegin, *dend, *data, *respdata;
		uint32_t dsize;
		int32_t sendRequest ( uint32_t cmd, uint32_t flags = 0, const void *reqReq = NULL, uint32_t reqRegSize = 0, const void *reqVdata = NULL, 
							  uint32_t reqVdataLen = 0, uint32_t respRegSize = 0, void *respVdata = NULL, uint32_t respVdataSize = 0 );	
    public:
        CSafeClient ( void );
        virtual ~CSafeClient ( void );
        virtual int32_t close ( uint32_t flags = 0 );
        virtual int32_t open ( const string &location, uint32_t flags = 0, uint32_t hash_key = 0 );
		virtual int32_t getInfo ( CSafeStorageInfo &info );
        virtual int32_t commit ( void );
        virtual int32_t rollback ( void );
        virtual int32_t verify ( uint32_t flags = 0 );
        virtual int32_t write ( tserial_t serial, const void *data, uint32_t dlen, uint32_t flags = 0 );
        virtual int32_t read ( tserial_t &serial, void *data, uint32_t dlen, uint32_t flags = 0 );
        virtual int32_t readLogReg ( tseq_t seq, tserial_t &serial, uint8_t &type, uint32_t flags = 0 );
        virtual int32_t readLog ( tseq_t seq, void *data, uint32_t dlen, uint32_t flags = 0 );
        virtual int32_t applyLog ( const void *data, uint32_t dlen, uint32_t flags = 0 );
		virtual int32_t getParam ( const string &name );
		virtual int32_t setParam ( const string &name, int32_t value );
		virtual int32_t createListener ( const string &params, ISafeStorageListener **ltn = NULL ) 	CSTORAGE_METHOD_NOT_IMPL;
		virtual int32_t createReplica ( const string &params, ISafeStorageReplica **rpl = NULL ) 	CSTORAGE_METHOD_NOT_IMPL;
		virtual int32_t setCallback ( tsafestorage_callback_t cb ) 									CSTORAGE_METHOD_NOT_IMPL;		
};

#endif
