#ifndef __BASE_SAFE_FILE__H__
#define __BASE_SAFE_FILE__H__

#include <stdint.h>
#include <string>
using namespace std;

#define PLAST_U32(P,L) (uint32_t *)(((uint8_t *)P)+(L-4)) 

#include <data_array.h>
#include <safe_file_exception.h>

#define F_CSFILE_WR             0x0001
#define F_CSFILE_CREATE         0x0002
#define F_CSFILE_DYNAMIC        0x0004
#define F_CSFILE_EXCEPTIONS     0x0008
#define F_CSFILE_AUTOSYNC       0x0010
#define F_CSFILE_EMPTY_DATA     0x0020
#define F_CSFILE_IGNORE_CRC     0x0040
#define F_CSFILE_CHECK_FLAGS    0x0080

#define C_CSFILE_ENDFILE   0x4000000000000000ULL
#define C_CSFILE_CURRPOS   0x4000000000000001ULL

#define E_CSFILE_EOF                -40
#define E_CSFILE_OPEN_FILE          -50
#define E_CSFILE_SEEK               -51
#define E_CSFILE_READ_SEEK          -52
#define E_CSFILE_WRITE_SEEK         -53
#define E_CSFILE_CRC                -54
#define E_CSFILE_READ               -55
#define E_CSFILE_WRITE              -56
#define E_CSFILE_INV_ACCESS         -57
#define E_CSFILE_DATA_TOO_BIG       -58
#define E_CSFILE_COPY_NOT_ALLOWED   -60
#define E_CSFILE_EMPTY_DATA         -70
#define E_CSFILE_CHANGE_FLAGS		-80

#define MAX_BASE_SAFE_FILE_ALIAS_LEN	4

#ifndef D_BSAFEFILE_DEFAULT_DELTA_PAGE
#define D_BSAFEFILE_DEFAULT_DELTA_PAGE	1024
#endif

#ifndef D_BSAFEFILE_DEFAULT_DATA_SIZE
#define D_BSAFEFILE_DEFAULT_DATA_SIZE	1024
#endif
	
#ifndef D_BSAFEFILE_DEFAULT_CACHE
#define D_BSAFEFILE_DEFAULT_CACHE		8192
#endif

typedef uint32_t tcrc_t;

class CBaseSafeFile
{
    protected:
        int32_t fd;
        bool f_write_mode;
        bool f_new_file;
        uint8_t *cache;
        int32_t cache_size;
        int64_t cache_offset;
        int32_t cache_len;
        int64_t current_offset;
        int64_t last_offset;
        uint8_t *work_area;
        int32_t work_area_size;
		int32_t max_data_size;
        int32_t flags;
		int32_t delta_page;
		uint8_t crc_zero_flag;
		int32_t alignment;
        string filename;
		char alias[MAX_BASE_SAFE_FILE_ALIAS_LEN+1];

		const char *autoAlias ( const char *filename );
        int32_t fillWorkArea ( int32_t len, const CDataArray &darray, int32_t offset_len = -1);
        int64_t writeWorkArea ( int64_t loff, int32_t len, int32_t skip = 0, int64_t *last_loff = NULL );
        int64_t readWorkArea ( int64_t loff, uint32_t flags, int32_t len, int32_t skip = 0, int64_t *last_loff = NULL );
        tcrc_t crc32 ( void *data, int32_t dlen );
        void allocateMemory ( void );
        void clearCache ( int64_t _loff = 0, int32_t len = -1);
        int64_t seek ( int64_t loff, int32_t len = 0, bool write = false );
		const char *offsetStr ( int64_t loff );
		void checkWorkAreaSize ( int32_t len );
		void allocateWorkArea ( void );	
		void allocateCache ( void );	
		
    public:
        CBaseSafeFile ( uint32_t flags = 0, int32_t cache_size = D_BSAFEFILE_DEFAULT_CACHE, int32_t max_data_size = D_BSAFEFILE_DEFAULT_DATA_SIZE, 
						int32_t delta_page = D_BSAFEFILE_DEFAULT_DELTA_PAGE, int32_t alignment = 1 );
        CBaseSafeFile ( const CBaseSafeFile &from );
        virtual ~CBaseSafeFile ();
        int32_t close ( uint32_t flags = 0 );
        int32_t open ( const string &filename, uint32_t flags = 0, uint32_t perms = 0660 );
        int32_t read ( int64_t loff, uint32_t flags, void *data, int32_t dlen );
        int32_t read ( int64_t loff, void *data, int32_t dlen );
        int32_t write ( int64_t loff, const void *data, int32_t dlen );
        int32_t write ( int64_t loff, uint32_t flags, const void *data, int32_t dlen );
        int32_t write ( int64_t loff, uint32_t flags, const CDataArray &darray );
        int32_t read ( int64_t loff, uint32_t flags, CDataArray  &darray );
        void sync ( void );
        void goTop ( void );
        int32_t sizeOf ( int32_t size );
        int64_t currentOffset ( void ) { return current_offset; };
        int64_t lastOffset ( void ) { return last_offset; };
		void setMaxDataSize ( int32_t value );
		void setCacheSize ( int32_t value );
		int32_t getMaxDataSize ( void ) { return max_data_size; };
		int32_t getCacheSize ( void ) { return cache_size; };
		int32_t setFlags ( uint32_t flags );
		const string &getFilename ( void ) { return filename; };
		void findLastSignatureReg ( int32_t max_size, uint32_t signature, uint32_t signature_mask, int32_t signature_offset );
	
};

#endif


