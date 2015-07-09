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
		int32_t sendRequest ( const void *req, uint32_t reqlen, const void *req_vdata, uint32_t req_vdatalen, void *resp, uint32_t resplen, 
						      const void *resp_vdata = NULL, int32_t *resp_vdatalen = NULL );
	
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
		virtual int32_t goTop ( uint32_t flags = 0 );
		virtual int32_t goPos ( tserial_t serial, uint32_t flags = 0 );
		virtual int32_t getParam ( const string &name );
		virtual int32_t setParam ( const string &name, int32_t value );
		virtual int32_t createListener ( const string &params, ISafeStorageListener **ltn = NULL ) 	CSTORAGE_METHOD_NOT_IMPL;
		virtual int32_t createReplica ( const string &params, ISafeStorageReplica **rpl = NULL ) 	CSTORAGE_METHOD_NOT_IMPL;
		virtual int32_t setCallback ( tsafestorage_callback_t cb ) 									CSTORAGE_METHOD_NOT_IMPL;		
};

#endif
