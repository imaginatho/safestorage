#ifndef __SAFE_FILE__H__
#define __SAFE_FILE__H__

#include <base_safe_file.h>
#include <safe_exception.h>
#include <c_log.h>
#include <typeinfo>

template <class R>
class CSafeFile: public CBaseSafeFile
{
    public:
		CSafeFile ( uint32_t flags = 0, int32_t cache_size = D_BSAFEFILE_DEFAULT_CACHE, int32_t max_data_size = 0, 
					int32_t delta_page = D_BSAFEFILE_DEFAULT_DELTA_PAGE, int32_t alignment = 1 );
        virtual ~CSafeFile ();
        int32_t read ( int64_t loff, R &data );
        int32_t write ( int64_t loff, const R &data );
        int32_t read ( int64_t loff, R &data , void *ddata, int32_t dlen );
        int32_t write ( int64_t loff, const R &data, const void *ddata, int32_t dlen );
        int32_t write ( const R &data, const void *ddata = NULL, int32_t dlen = 0 );
        int32_t readIndex ( int32_t index, R &data );
        int32_t writeIndex ( int32_t index, const R &data );
        int32_t sizeOf ( void );
		void findLastSignatureReg ( uint32_t max_size, uint32_t signature, uint32_t signature_mask, int32_t signature_offset );
};

template <class R>
CSafeFile<R>::CSafeFile ( uint32_t flags, int32_t cache_size, int32_t max_data_size, int32_t delta_page, int32_t alignment )
    :CBaseSafeFile(flags, cache_size, sizeof(R) + max_data_size, delta_page, alignment)
{
}

template <class R>
CSafeFile<R>::~CSafeFile ( void )
{
}
//
/*
template <class H, class R>
R *CSafeFile<H,R>::operator[] ( tregpos_t pos )
{
    if (LoadRegister(pos) < 0)
        return NULL;
    return GetCurrentRegister();
}
*/
template <class R>
int32_t CSafeFile<R>::read ( int64_t loff, R &data )
{
    return CBaseSafeFile::read(loff, &data, sizeof(R));
}

template <class R>
int32_t CSafeFile<R>::write ( int64_t loff, const R &data )
{
    return CBaseSafeFile::write(loff, &data, sizeof(R));
}

template <class R>
int32_t CSafeFile<R>::read ( int64_t loff, R &data, void *ddata, int32_t dlen )
{
    C_LOG_DEBUG("Read sizeof(R)=%d %p %p[%d]", sizeof(R), &data, ddata, dlen);
    CDataArray darray(2, &data, sizeof(R), _DAPSF(ddata, dlen, F_CDARRAY_VLEN));
    return CBaseSafeFile::read(loff, F_CSFILE_DYNAMIC, darray);
}

template <class R>
int32_t CSafeFile<R>::write ( int64_t loff, const R &data, const void *ddata, int32_t dlen )
{
    C_LOG_DEBUG("Write sizeof(R)=%d %p %p[%d]", sizeof(R), &data, ddata, dlen);
    CDataArray darray(2, &data, sizeof(R), ddata, dlen);
//    return CBaseSafeFile::Write(loff, (ddata && dlen ? F_CSFILE_DYNAMIC : 0 ), darray);
    return CBaseSafeFile::write(loff, F_CSFILE_DYNAMIC, darray);
}

template <class R>
int32_t CSafeFile<R>::write ( const R &data, const void *ddata, int32_t dlen )
{
    C_LOG_DEBUG("Write sizeof(R)=%d %p dlen=%d", sizeof(R), &data, dlen);
    CDataArray darray(ddata ? 2 : 1, &data, sizeof(R), ddata, dlen);
//    return CBaseSafeFile::Write(C_CSFILE_ENDFILE,  (ddata && dlen ? F_CSFILE_DYNAMIC : 0 ), darray);
    return CBaseSafeFile::write(C_CSFILE_ENDFILE,  F_CSFILE_DYNAMIC, darray);
}

template <class R>
int32_t CSafeFile<R>::readIndex ( int32_t index, R &data )
{
    return CBaseSafeFile::read((int64_t)index * (int64_t)sizeOf(), F_CSFILE_EMPTY_DATA, &data, sizeof(R));
}

template <class R>
int32_t CSafeFile<R>::writeIndex ( int32_t index, const R &data )
{
    return CBaseSafeFile::write((int64_t)index * (int64_t)sizeOf(), &data, sizeof(R));
}

template <class R>
int32_t CSafeFile<R>::sizeOf ( void )
{
    return CBaseSafeFile::sizeOf(sizeof(R));
}

template <class R>
void CSafeFile<R>::findLastSignatureReg ( uint32_t max_size, uint32_t signature, uint32_t signature_mask, int32_t signature_offset )
{
	CBaseSafeFile::findLastSignatureReg(max_size, signature, signature_mask, signature_offset);
}

/*
template <class H, class R>
H *CSafeFile<H,R>::GetHeader ( void ) 
{ 
    return (H *)CBaseSafeFile::GetHeader();
};

template <class H, class R>
R *CSafeFile<H,R>::GetCurrentRegister ( void )
{
    return (R *)CBaseSafeFile::GetCurrentRegister();
}
*/
/*
template <class H, class R>
R *CSafeFile<H,R>::LoadRegister ( tregpos_t pos, bool cached )
{
    return (R *)CBaseSafeFile::LoadRegister(pos, cached);
}
*/
/*
template <class H, class R>
R *CSafeFile<H,R>::LoadRegister ( int64_t loffset )
{
    return (R *)CBaseSafeFile::LoadRegister(loffset);
}

template <class H, class R>
int32_t CSafeFile<H,R>::SaveHeader ( H *header )
{
    if (!header)
        return CBaseSafeFile::SaveHeader();
    return CBaseSafeFile::SaveHeader((tsfheader_t *)header);
}

template <class H, class R>
int32_t CSafeFile<H,R>::SaveRegister ( R *reg, tregpos_t pos )
{
    return CBaseSafeFile::SaveRegister((tsfreg_t *)reg, pos); 
}

template <class H, class R>
int32_t CSafeFile<H,R>::SaveRegister ( R *reg, void *data, int32_t dlen, int64_t loffset )
{
    return CBaseSafeFile::SaveRegister((tsfreg_t *)reg, data, dlen, loffset); 
}
*/
#endif
