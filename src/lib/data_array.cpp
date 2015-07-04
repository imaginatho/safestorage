#include <data_array.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include <typeinfo>
#include <string>
#include <exception>
#include <iostream>
#include <sstream>
#include <c_log.h>

using namespace std;

CDataArray::CDataArray ( int32_t count, ... )
{
    C_LOG_DEBUG("CDataArray[%p]::(%d,....)", this, count);
//    uint8_t *data = (uint8_t *)malloc((sizeof(void *)+sizeof(uint8_t)+sizeof(int32_t))*count);
    
    uint8_t *data = (uint8_t *)malloc(1024);
	_fsize = 0;    
    _datas = (void **) data;
    data += (sizeof(void *)*count);
    _lens = (int32_t *) data; 
    data += (sizeof(int32_t)*count);
    _flags = data;
    _size = 0;
    _count = count;
     
    uint32_t tmp;
    bool fixed = true;
    va_list va_args;
    va_start(va_args, count);
    for (int32_t index = 0; index < count; ++index)
    {
        _datas[index] = va_arg(va_args, void *);
        tmp = va_arg(va_args, uint32_t);    
        _flags[index] = (tmp >> 24) & 0xFF;
        tmp = 0x00FFFFFF & tmp;
        _lens[index] = tmp;
        C_LOG_DEBUG("CDataArray[%p]::CDataArray _lens[%d]=%d _lens=%p", this, index, tmp, _lens);
        _size += tmp;
        
        if (_flags[index] & F_CDARRAY_VLEN)
            fixed = false;
            
        if (fixed)
            _fsize += tmp;
    }
    va_end(va_args);    
}

CDataArray::CDataArray ( const CDataArray &source )
{
    C_LOG_DEBUG("CDataArray::Copy (%p, %p)", this, &source);
    int32_t bytes = (sizeof(void *)+sizeof(uint8_t)+sizeof(int32_t))*source._count;
    C_LOG_DEBUG("CDataArray[%p]::Copy bytes=%d", this, bytes);
    uint8_t *data = (uint8_t *)malloc(bytes);

    _datas = (void **)data;
    memcpy(_datas, source._datas, bytes);     
    _count = source._count;
    _size = source._size;
    _fsize = source._fsize;
    data += (sizeof(void *)*_count); 
    _lens = (int32_t *) data; 
    data += (sizeof(int32_t)*_count);
    _flags = data;    
}

CDataArray::~CDataArray ( void )
{
	C_LOG_DEBUG("CDataArray[%p]::~CDataArray()", this)
    free(_datas);
}

int32_t CDataArray::count ( void ) const
{
    return _count;
}

int32_t CDataArray::size ( void ) const 
{ 
    return _size; 
};

void *CDataArray::data ( int32_t index ) const 
{ 
    return (index >= 0 && index < _count)?_datas[index]:NULL; 
}   

int32_t CDataArray::len ( int32_t index ) const
{ 
	int32_t result = (index >= 0 && index < _count)?_lens[index]:0;
//	C_LOG_DEBUG("CDataArray[%p]::len(%d)=%d _lens=%p", this, index, result, _lens);
    return result;
}   

uint8_t CDataArray::flags ( int32_t index ) const 
{ 
    return (index >= 0 && index < _count)?_flags[index]:0; 
} 

int32_t CDataArray::setFromBuffer ( void *data, int32_t dlen )
{
	C_LOG_DEBUG("CDataArray[%p]::setFromBuffer(%p, %d)", this, data, dlen);
	uint8_t *pdata = (uint8_t *)data;
    int32_t bcpy, idata;
    uint8_t *item, *pmem = NULL;

/*    for (int32_t i = 0; i < dlen; ++i)
    {
        printf("%02X", ((uint8_t *)data)[i]);
        if ((i & 0x0F) == 0x0F) printf("\n");
    }    
    printf("\n");*/
    for (int32_t index = 0; index < _count ; ++index)
    {
        item = (uint8_t *)_datas[index];
        if (_flags[index] & F_CDARRAY_VLEN)        
        {    
            if (_flags[index] & F_DYNAMIC_ALLOCATE)
            {
                pmem = (uint8_t *)malloc(dlen);
                *((uint8_t **)_datas[index]) = pmem;
                item = pmem;
                bcpy = dlen;
            }   
            else {
            	C_LOG_DEBUG("dlen=%d _lens[%d]=%d", dlen, index, _lens[index]);
            	bcpy = (dlen > _lens[index] ? _lens[index]:dlen);
            }
            idata = dlen;
        }
        else {
            idata = bcpy = _lens[index];
        }
        C_LOG_DEBUG("dlen:%d bcpy:%d item:%p pdata:%d", dlen, bcpy, item, (int)(pdata-(uint8_t *)data));
        if (dlen < bcpy)
        {
        	C_LOG_DEBUG("**interrupt** dlen(%d) < bcpy(%d)", dlen, bcpy);
            return -1;
        }
        memcpy(item, pdata, bcpy);
        pdata += idata;
        dlen -= idata;
    }
    return 0;
}

int32_t CDataArray::fsize ( void ) const 
{ 
    return _fsize; 
}  

string CDataArray::debug ( void ) const
{
    stringstream ss;
    ss << "[" << _count << "]={";
    for (int32_t index = 0; index < _count; ++index)
        ss << "(" << _datas[index] << ","  << _lens[index] << "," << (int)_flags[index] << ")";
    ss << "}";
    return ss.str();
}
