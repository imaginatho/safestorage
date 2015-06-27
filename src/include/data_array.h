#ifndef __DATA_ARRAY__H__
#define __DATA_ARRAY__H__

#include <stdio.h>
#include <unistd.h>
#include <stdint.h>

#include <string>
#include <exception>

#define _DAP(X) &X,sizeof(X)
#define _DAPF(X,F) &X,(sizeof(X)|((F) << 24))
#define _DAPSF(X,S,F) X,(S|((F) << 24))
#define _DAPS(X,S) &X,S
#define _DASF(S,F) (S|((F) << 24))

#define F_CDARRAY_VLEN 0x01
#define F_DYNAMIC_ALLOCATE 0x02

using namespace std;
 
class CDataArray
{
    protected:
        int32_t _count;
        int32_t _size;
        int32_t _fsize;
        uint8_t *_flags;
        int32_t *_lens;
        void **_datas;        
    public:
        CDataArray ( int32_t count, ... );
        CDataArray ( const CDataArray &source );
        virtual ~CDataArray ( void );
        int32_t size ( void ) const;
        int32_t count ( void ) const;
        void *data ( int32_t index ) const;  
        int32_t len ( int32_t index ) const;  
        uint8_t flags ( int32_t index ) const;
        int32_t setFromBuffer ( void *data, int32_t dlen );
        int32_t fsize ( void ) const;
        string debug ( void ) const; 
};

#endif
