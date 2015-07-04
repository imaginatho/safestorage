#ifndef __SAFE_STORAGE_BRIDGE_H__
#define __SAFE_STORAGE_BRIDGE_H__

#include <stdint.h>
#include <string>
#include <list>
#include <set>
using namespace std;

#include <safe_file_exception.h>
#include <base_safe_file.h>
#include <safe_file.h>
#include <safe_storage.h>

#define __BRIDGE_RET_STG_CALL(OP,X) uint32_t __opst = O_CSTORAGE_BRIDGE_##OP;\
									if (!allowed(__opst)) return E_CSTORAGE_OPERATION_NOT_ALLOWED;\
									if (!storage) return E_CSTORAGE_NOT_OPENED;\
									return storage->X
									
#define __BM_FLAGS muteFlags(flags, __opst)

#define O_CSTORAGE_BRIDGE_OPEN                0x00000001
#define O_CSTORAGE_BRIDGE_CLOSE               0x00000002
#define O_CSTORAGE_BRIDGE_COMMIT              0x00000004
#define O_CSTORAGE_BRIDGE_ROLLBACK            0x00000008
#define O_CSTORAGE_BRIDGE_VERIFY              0x00000010
#define O_CSTORAGE_BRIDGE_WRITE               0x00000020
#define O_CSTORAGE_BRIDGE_READ                0x00000040
#define O_CSTORAGE_BRIDGE_READ_LOG_REG        0x00000080
#define O_CSTORAGE_BRIDGE_READ_LOG            0x00000100
#define O_CSTORAGE_BRIDGE_GO_TOP              0x00000200
#define O_CSTORAGE_BRIDGE_GO_POS              0x00000400
#define O_CSTORAGE_BRIDGE_GET_PARAM           0x00000800
#define O_CSTORAGE_BRIDGE_SET_PARAM           0x00001000
#define O_CSTORAGE_BRIDGE_APPLY_LOG           0x00002000
#define O_CSTORAGE_BRIDGE_GET_INFO            0x00004000
#define O_CSTORAGE_BRIDGE_CREATE_LISTENER     0x00008000
#define O_CSTORAGE_BRIDGE_CREATE_REPLICA      0x00010000
#define O_CSTORAGE_BRIDGE_SET_CALLBACK        0x00020000
                                              
class CSafeStorageBridge: public ISafeStorage
{
	protected:
		ISafeStorage *storage;
		virtual uint32_t muteFlags ( uint32_t flags, uint32_t operation ) { return flags; };
		virtual bool allowed ( uint32_t operation ) { return true; };
    public:
		CSafeStorageBridge ( void ) { storage = NULL; };
		virtual ~CSafeStorageBridge () {};
		ISafeStorage *getStorage ( void ) { return storage; };

        virtual int32_t close ( uint32_t flags = 0 ) 													{__BRIDGE_RET_STG_CALL(CLOSE, close(__BM_FLAGS));};
        virtual int32_t open ( const string &location, uint32_t flags = 0, uint32_t hash_key = 0 ) 		{__BRIDGE_RET_STG_CALL(OPEN, open(location, __BM_FLAGS, hash_key));};
        virtual int32_t getInfo ( CSafeStorageInfo &info )											    {__BRIDGE_RET_STG_CALL(GET_INFO, getInfo(info));};
        virtual int32_t commit ( void ) 																{__BRIDGE_RET_STG_CALL(COMMIT, commit());};
        virtual int32_t rollback ( void ) 																{__BRIDGE_RET_STG_CALL(ROLLBACK, rollback());};
        virtual int32_t verify ( uint32_t flags = 0 ) 													{__BRIDGE_RET_STG_CALL(VERIFY, verify(__BM_FLAGS));};
        virtual int32_t write ( tserial_t serial, const void *data, uint32_t dlen, uint32_t flags = 0 ) {__BRIDGE_RET_STG_CALL(WRITE, write(serial, data, dlen, __BM_FLAGS));};
        virtual int32_t read ( tserial_t &serial, void *data, uint32_t dlen, uint32_t flags = 0 ) 		{__BRIDGE_RET_STG_CALL(READ, read(serial, data, dlen, __BM_FLAGS));};
        virtual int32_t readLogReg ( tseq_t seq, tserial_t &serial, uint8_t &type, uint32_t flags = 0 ) {__BRIDGE_RET_STG_CALL(READ_LOG_REG, readLogReg(seq, serial, type, __BM_FLAGS));};
        virtual int32_t readLog ( tseq_t seq, void *data, uint32_t dlen, uint32_t flags = 0 )			{__BRIDGE_RET_STG_CALL(READ_LOG, readLog(seq, data,dlen, __BM_FLAGS));};
        virtual int32_t applyLog ( const void *data, uint32_t dlen, uint32_t flags = 0 )				{__BRIDGE_RET_STG_CALL(APPLY_LOG, applyLog( data,dlen, __BM_FLAGS));};
		virtual int32_t goTop ( uint32_t flags = 0 )													{__BRIDGE_RET_STG_CALL(GO_TOP, goTop(__BM_FLAGS));};
		virtual int32_t goPos ( tserial_t serial, uint32_t flags = 0 )									{__BRIDGE_RET_STG_CALL(GO_POS, goPos(serial, __BM_FLAGS));};
		virtual int32_t getParam ( const string &name )													{__BRIDGE_RET_STG_CALL(GET_PARAM, getParam(name));};
		virtual int32_t setParam ( const string &name, int32_t value )									{__BRIDGE_RET_STG_CALL(SET_PARAM, setParam(name, value));};
		virtual int32_t createListener ( const string &params, ISafeStorageListener **ltn )				{__BRIDGE_RET_STG_CALL(CREATE_LISTENER, createListener(params, ltn));};
		virtual int32_t createReplica ( const string &params, ISafeStorageReplica **rpl )				{__BRIDGE_RET_STG_CALL(CREATE_REPLICA, createReplica(params, rpl));};
		virtual int32_t setCallback( tsafestorage_callback_t cb )										{__BRIDGE_RET_STG_CALL(SET_CALLBACK, setCallback(cb));};
};

#endif /* __SAFE_STORAGE_BRIDGE_H__ */
