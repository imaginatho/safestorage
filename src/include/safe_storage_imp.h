#ifndef __SAFE_STORAGE__H__
#define __SAFE_STORAGE__H__

#include <stdint.h>
#include <string>
#include <list>
#include <map>
#include <set>
using namespace std;

#include <safe_file_exception.h>
#include <base_safe_file.h>
#include <safe_file.h>
#include <safe_storage.h>

#define CSTORAGE_SIGNATURE          	0x51213298
#define CSTORAGE_SIGNATURE_TYPE_MASK    0x00000007
#define CSTORAGE_SIGNATURE_SIGN_MASK    0xFFFFFFF8

#define D_CSTORAGE_INDEX_CACHE		0
#define D_CSTORAGE_LOG_CACHE		0

#define F_CSTORAGE_DF_AUTO_COMMIT	0x01

#ifndef C_STORAGE_STATUS_FREQ
#define C_STORAGE_STATUS_FREQ		50
#endif

#ifndef D_CSTORAGE_INDEX_CACHE
#define D_CSTORAGE_INDEX_CACHE		(16*1024)	
#endif

#ifndef D_CSTORAGE_LOG_CACHE
#define D_CSTORAGE_LOG_CACHE		(4*1024)
#endif

#ifndef D_CSTORAGE_DATA_INIT_SIZE
#define D_CSTORAGE_DATA_INIT_SIZE	(32*1024)
#endif

#ifndef D_CSTORAGE_DATA_DELTA_SIZE
#define D_CSTORAGE_DATA_DELTA_SIZE	(16*1024)
#endif

#ifndef C_DIRTY_SERIAL_SIZE
#define C_DIRTY_SERIAL_SIZE (4*1024)
#endif

struct CSafeStorageDataReg
{
    uint32_t signature;
    tserial_t serial;
    tseq_t sequence;
    int32_t len;
    uint8_t flags;
} __attribute__ ((packed));

struct CSafeStorageState
{
    uint32_t hash_key;
    uint32_t last_updated;
    int32_t count;
    int32_t commit_count;
    int32_t rollback_begin_count;
    int32_t rollback_end_count;
    int32_t data_count;
    int8_t last_offset_index;

    tseq_t last_commit_sequence;
    tseq_t last_rollback_sequence;
    tseq_t last_close_sequence;
    tseq_t last_sequence;

    uint64_t last_close_offset;
    uint64_t last_offsets[N_SS_LAST_OFFSETS];
} __attribute__ ((packed));


struct CSafeStorageLogReg
{    
	int64_t offset;
    tserial_t serial; 
    uint8_t type;
} __attribute__ ((packed));

struct CSafeStorageIndexReg
{
    int64_t offsets[2];
    tseq_t sequences[2];
    uint8_t flags[2];
} __attribute__ ((packed));

typedef list<CBaseSafeFile *> CBaseSafeFileList;

class CSafeStorage: public ISafeStorage
{
    protected:
        CSafeFile<CSafeStorageIndexReg> findex;
        CSafeFile<CSafeStorageLogReg> flog;
        CSafeFile<CSafeStorageDataReg> fdata;
        CSafeFile<CSafeStorageDataReg> ftrans;
        CSafeFile<CSafeStorageState> fstate;
		
		uint32_t dirtySerialIndex;
		tserial_t *dirtySerials;
		uint32_t dirtySerialSize;
		
        tseq_t last_sequence;
		tserial_t cursor;
        CBaseSafeFileList files;
        CSafeStorageState state;
        int64_t last_offset;
        bool fauto_commit;
		bool rdwr;
		uint8_t mode;
        
        int32_t checkFilesPresent ( const string &filename, int32_t mode );
        int32_t checkAnyFilePresent ( const string &filename );
        void saveState ( bool sync = false );
        void loadState ( void );
        int32_t writeHashKey ( uint32_t hash_key, uint32_t flags = 0 );
        int32_t checkHashKey ( void );
        int64_t writeData ( CSafeStorageDataReg &r_data, const void *data = NULL, uint32_t dlen = 0 );
        int32_t recoverDirtySerials ( void );
        void setOldestOffset ( CSafeStorageIndexReg &index, tserial_t serial, int64_t offset, tseq_t sequence, uint32_t flags, bool init );
        void setNewestOffset ( CSafeStorageIndexReg &index, tserial_t serial, int64_t offset, tseq_t sequence, uint32_t flags );
        int64_t getLastOffset ( CSafeStorageIndexReg &index, tserial_t serial, uint32_t flags = 0 );
        int32_t writeFiles ( CSafeStorageDataReg &r_data, const void *data = NULL, uint32_t dlen = 0 );
        void writeSyncStateLogSync ( CSafeStorageLogReg &r_log );
        int64_t getLastOffset ( void );
		void clearDirtySerials ( void );
		void addDirtySerial ( tserial_t serial );
		string normalizeFilename ( const string &filename );	
        int32_t getHashKey ( uint32_t &hash_key );

        int32_t create ( const string &filename, uint32_t flags = 0, uint32_t hash_key = 0 );
		
    public:
        CSafeStorage ( void );
        virtual ~CSafeStorage ();
        void sync ( bool full = false );
        virtual uint32_t generateHashKey ( void );
        static int32_t removeFiles ( const string &filename );
        void dumpState ( void );

		int32_t setFlags ( uint32_t flags );

        // ISafeStorage
        virtual int32_t close ( uint32_t flags = 0 );
        virtual int32_t open ( const string &filename, uint32_t flags = 0, uint32_t hash_key = 0 );
        virtual int32_t getInfo ( CSafeStorageInfo &info );
        virtual int32_t rebuild ( const string &filename, uint32_t flags = 0 );
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

		virtual int32_t createListener ( const string &params, ISafeStorageListener **ltn );
		virtual int32_t createReplica ( const string &params, ISafeStorageReplica **rpl );
		virtual int32_t setCallback ( tsafestorage_callback_t cb );
		
		void findLastSignatureReg ( int32_t max_size );

};

#endif


