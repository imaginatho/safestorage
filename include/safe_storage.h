#ifndef __I_SAFE_STORAGE_H__
#define __I_SAFE_STORAGE_H__

#include <stdint.h>
#include <string>

#define SAFE_STORAGE_VERSION	0x0101

typedef int32_t tseq_t;
typedef int32_t tserial_t;
typedef int8_t tslot_t;

#ifndef N_SS_LAST_OFFSETS
#define N_SS_LAST_OFFSETS 16
#endif

struct CSafeStorageInfo
{
    uint32_t hash_key;
    uint32_t last_updated;
	uint32_t version;
    int32_t count;
    int32_t commit_count;
    int32_t rollback_begin_count;
    int32_t rollback_end_count;

    tseq_t last_commit_sequence;
    tseq_t last_rollback_sequence;
    tseq_t last_sequence;

} __attribute__ ((packed));

struct CSafeStorageLogInfo
{
    tserial_t serial;
    tseq_t    sequence;
    int32_t   len;
    uint32_t  flags;
    uint8_t   type;
} __attribute__ ((packed));

#define E_CSTORAGE_OK               				0
#define E_CSTORAGE_ERROR           					-500
#define E_CSTORAGE_OPEN             				-50
#define E_CSTORAGE_EOF	             				-40
#define E_CSTORAGE_CREATE           				-501
#define E_CSTORAGE_CREATE_FILE_EXISTS				-502
#define E_CSTORAGE_OPEN_FILE_NO_ACCESS				-503
#define E_CSTORAGE_DATA_READ						-504
#define E_CSTORAGE_SERIAL_NOT_FOUND					-505
#define E_CSTORAGE_NOT_OPENED						-506
#define E_CSTORAGE_ALREADY_OPENED					-507
#define E_CSTORAGE_NO_ROLLBACK_IN_AUTOCOMMIT		-508
#define E_CSTORAGE_SEQUENCE_NOT_FOUND				-509
#define E_CSTORAGE_NOT_ALLOWED_IN_READ_MODE			-510
#define E_CSTORAGE_ONLY_ONE_WRITER              	-511
#define E_CSTORAGE_UNKNOWN_COMMAND              	-512
#define E_CSTORAGE_LOCKED_REGISTER				    -513
#define E_CSTORAGE_OPERATION_NOT_ALLOWED			-514
#define E_CSTORAGE_OPERATION_NOT_IMPLEMENTED		-515
#define E_CSTORAGE_NOT_ENOUGH_DATA					-516
#define E_CSTORAGE_NOT_VALID_DATA					-517

#define E_CSTORAGE_ONLT_ONE_WRITTER		-600

#define F_CSTORAGE_WR       		0x00000001
#define F_CSTORAGE_CREATE			0x00000002
#define F_CSTORAGE_SET_HASH_KEY		0x00001000
#define F_CSTORAGE_AUTO_COMMIT 		0x00010000

#define F_CSTORAGE_READ_MODE_MASK	0x00300000
#define F_CSTORAGE_COMMITED_READ	0x00000000
#define F_CSTORAGE_DIRTY_READ		0x00100000
#define F_CSTORAGE_FULL_DIRTY_READ	0x00200000

#define E_CSTORAGE_FAIL_HK_INDEX    0x0001
#define E_CSTORAGE_FAIL_HK_STATE    0x0002
#define E_CSTORAGE_FAIL_HK_LOG      0x0004

#define T_CSTORAGE_COMMIT                 0x01
#define T_CSTORAGE_WRITE                  0x02
#define T_CSTORAGE_ROLLBACK_BEGIN         0x03
#define T_CSTORAGE_ROLLBACK_END	          0x04
#define T_CSTORAGE_STATUS		          0x05

#define F_CSTORAGE_STD_INSTANCE				0x00000000
#define F_CSTORAGE_MASTER_INSTANCE			0x00000100
#define	F_CSTORAGE_SLAVE_INSTANCE			0x00000200

#define E_CB_REPLICA_FAIL					-1000

typedef int32_t (* tsafestorage_callback_t)(int32_t, void *);

class ISafeStorageListener
{
	public:
		virtual ~ISafeStorageListener ( void ) {};
		virtual int32_t stop ( void ) = 0;
};

class ISafeStorageReplica 
{
	public:
		virtual ~ISafeStorageReplica ( void ) {};
		virtual int32_t stop ( void ) = 0;
};

class ISafeStorage
{
    public:
		virtual ~ISafeStorage ( void ) {};
        virtual int32_t close ( uint32_t flags = 0 ) = 0;
        virtual int32_t open ( const std::string &location, uint32_t flags = 0, uint32_t hash_key = 0 ) = 0;
        virtual int32_t getInfo ( CSafeStorageInfo &info ) = 0;
        virtual int32_t commit ( void ) = 0;
        virtual int32_t rollback ( void ) = 0;
        virtual int32_t verify ( uint32_t flags = 0 ) = 0;
        virtual int32_t write ( tserial_t serial, const void *data, uint32_t dlen, uint32_t flags = 0 ) = 0;
        virtual int32_t read ( tserial_t &serial, void *data, uint32_t dlen, uint32_t flags = 0 ) = 0;
        virtual int32_t readLogReg ( tseq_t seq, tserial_t &serial, uint8_t &type, uint32_t flags = 0 ) = 0;
        virtual int32_t readLog ( tseq_t seq, void *data, uint32_t dlen, uint32_t flags = 0 ) = 0;
        virtual int32_t applyLog ( const void *data, uint32_t dlen, uint32_t flags = 0 ) = 0;
		virtual int32_t goTop ( uint32_t flags = 0 ) = 0;
		virtual int32_t goPos ( tserial_t serial, uint32_t flags = 0 ) = 0;
		virtual int32_t getParam ( const std::string &name ) = 0;
		virtual int32_t setParam ( const std::string &name, int32_t value ) = 0;
		virtual int32_t createListener ( const std::string &params, ISafeStorageListener **ltn = NULL )	= 0;
		virtual int32_t createReplica ( const std::string &params, ISafeStorageReplica **rpl = NULL ) = 0;	
		virtual int32_t setCallback ( tsafestorage_callback_t cb ) = 0;
		
		static  int32_t getLogReg ( const void *data, uint32_t dlen, CSafeStorageLogInfo &linfo );
};

ISafeStorage *createISafeStorage ( uint32_t flags = F_CSTORAGE_STD_INSTANCE, uint32_t version = SAFE_STORAGE_VERSION );

#endif
