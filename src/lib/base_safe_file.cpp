#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdarg.h>
#include <fcntl.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <sstream>
#include <string>
#include <exception>
using namespace std;

#include <base_safe_file.h>
#include <safe_file_exception.h>
#include <data_array.h>
#include <c_log.h>
#include <crc32.h>

#define WORK_AREA_RESERV_SIZE 128
#define D_BSFILE_FIND_LAST_BLOCK_SIZE	(32*1024)

/*
 * Constructor to create class that allow write and read to/from file in secure way, to
 * guarantee that information read is correct.
 *
 * @param flags to set default flags that applies to all operations
 * @param cache_size cache size in bytes
 * @param max_data_size maximum size of a record
 */
CBaseSafeFile::CBaseSafeFile ( uint32_t flags, int32_t cache_size, int32_t max_data_size, int32_t delta_page, int32_t alignment )
            :cache_size(cache_size), max_data_size(max_data_size), flags(flags), delta_page(delta_page), alignment(alignment)
{
	// initialize
    fd = -1;
	
    work_area = cache = NULL;
	if (max_data_size > 0) {
		work_area_size = WORK_AREA_RESERV_SIZE + max_data_size;			
	}
    last_offset = current_offset = 0;

    f_write_mode = f_new_file = false;

    // allocate memory
    allocateMemory();
}

/**
 * This method is defined to avoid make a copy of object
 */
CBaseSafeFile::CBaseSafeFile ( const CBaseSafeFile &from )
{
    throw CSafeFileException(__FILE__,__LINE__, E_CSFILE_COPY_NOT_ALLOWED);
}

/**
 * Allocate memory of work_area and cache.
 */
void CBaseSafeFile::allocateMemory ( void )
{
	allocateWorkArea();
	allocateCache();
}

void CBaseSafeFile::allocateCache ( void )
{
    // initialize cache information
    cache_len = 0;
    cache_offset = -1;

	// if cache have memory allocated, before free it.
    if (cache) {
        free(cache);
		cache = NULL;
	}
        
    // allocate memory to cache
    if (cache_size > 0)
        cache = (uint8_t *)malloc(cache_size);
}

void CBaseSafeFile::setMaxDataSize ( int32_t value )
{
	if (max_data_size == value) return;
	
	C_LOG_DEBUG("[%s] max_data_size = %d -> %d", alias, max_data_size, value);
	max_data_size = value;
	work_area_size = (max_data_size > 0) ? WORK_AREA_RESERV_SIZE + max_data_size : 0;
	allocateWorkArea();
}

void CBaseSafeFile::setCacheSize ( int32_t value )
{
	if (cache_size == value) return;
	
	C_LOG_DEBUG("[%s] CHANGE cache_size = %d -> %d", alias, cache_size, value);
	cache_size = value;
	allocateCache();
}

/**
 * Destructor, free cache and work area buffers
 */
CBaseSafeFile::~CBaseSafeFile ()
{
    if (cache)
		free(cache);
	
    if (work_area) 
		free(work_area);
}

/**
 * Force a synchronization of data, close file descriptor, an initialize all flags, prepared
 * to new open call.
 *
 * @params flags currently not used
 *
 * @return if ok returns zero, otherwise return error code.
 */

int32_t CBaseSafeFile::close ( uint32_t flags )
{
	// Force sync of data
	fsync(fd);

	// close file, if it's open
    if (fd >= 0)
        ::close(fd);

    // Reset flags and file description, to be ready for new open call.
    fd = -1;
    f_write_mode = false;
    f_new_file = false;

    return 0;
}

const char *CBaseSafeFile::autoAlias ( const char *filename )
{
	const char *ext;
	
	alias[0] = '\0';
	ext = strrchr(filename, '.');
	if (!ext) {
		return alias;
	}
	int32_t dlen = strlen(filename);
	++ext;
	if ((dlen - (int32_t)(ext-filename)) >= (int32_t)sizeof(alias)) {
		ext = filename;
		ext += (dlen+1-sizeof(alias));
	}
	strcpy(alias, ext);
	return alias;
}

/**
 * Method to open file where will store data
 * @param filename filename of file where store data
 * @param flags flags used in open, possible flags are:
 * 				F_CSFILE_CREATE (If file doesn't exists, create it),
 * 				F_CSFILE_WR (If it's possible write data),
 * 				F_CSFILE_EXCEPTIONS (When found errors produce an exception)
 * @param perms perms of file in case of file was create.
 *
 * @return error code, zero if operation it's Ok.
 */
int32_t CBaseSafeFile::open ( const string &filename, uint32_t flags, uint32_t perms )
{
    int32_t _flags = O_LARGEFILE;
    this->filename = filename;
	autoAlias(filename.c_str());
    
    // if file is opened, before close it.
    if (fd >= 0)
        ::close(fd);


	if (flags & F_CSFILE_CREATE) flags |= F_CSFILE_WR;
    // set flag of class to known if are in write mode
    f_write_mode = (flags & F_CSFILE_WR);
	C_LOG_DEBUG("[%s] f_write_mode=%s", alias, f_write_mode?"true":"false");
    
    // setting system call flags
    if (f_write_mode) _flags |= (O_NOATIME|O_RDWR);
    else _flags |= (O_RDONLY|O_NOATIME);
	
	if (f_write_mode && (flags & F_CSFILE_TRUNCATE)) _flags |= (O_TRUNC|O_CREAT);

    // this loop was implemented because first try to open file, without create
    // if it's not possible (error ENOENT), loop again to create file.
    do {     
        fd = ::open(filename.c_str(), _flags, perms);

        if (fd < 0) {
            // if open fails with ENOENT, check flags to known if could create file and
            // check if this last open try to create, if not continues.
            if ((!(flags & F_CSFILE_CREATE) || (_flags & O_CREAT)) && errno == ENOENT)
            {
                if (flags & F_CSFILE_EXCEPTIONS) 
                    throw CSafeFileException(__FILE__,__LINE__, E_CSFILE_OPEN_FILE);  
                return E_CSFILE_OPEN_FILE;
            }

            // setting flags for next loop where system call try to create file.
            f_new_file = true;
            _flags |= O_CREAT;
        }
    } while (fd < 0);

    return 0;
}

int32_t CBaseSafeFile::setFlags ( uint32_t flags )
{
	if (flags & F_CSFILE_CREATE) flags |= F_CSFILE_WR;
    bool new_write_mode = (flags & F_CSFILE_WR);

	if (new_write_mode == f_write_mode || fd < 0) {
		return 0;
	}

    int32_t _flags = O_LARGEFILE;

    if (new_write_mode) _flags |= (O_NOATIME|O_RDWR);
    else _flags |= (O_RDONLY|O_NOATIME);
	
    C_LOG_DEBUG("[%s] changeFlags f_write_mode=%s", alias, f_write_mode?"true":"false");
    
    // setting system call flags
    if (new_write_mode) _flags |= (O_NOATIME|O_RDWR);
    else _flags |= (O_RDONLY|O_NOATIME);

	if (flags & F_CSFILE_CHECK_FLAGS) {
		if (access(filename.c_str(), new_write_mode ? W_OK|R_OK : R_OK) == 0) return 0;
		else return E_CSFILE_CHANGE_FLAGS;
	}
	
    int nfd = ::open(filename.c_str(), _flags);
	if (nfd >= 0) {
		::close(fd);
		fd = nfd;
		f_write_mode = new_write_mode;		
		return 0;
	}
	
	return E_CSFILE_CHANGE_FLAGS;
}
 
/**
 * get quantity of bytes need to stored an information. This quantity depends if data it's dynamic (need to store the
 * length) or not.
 * @param size quantity of data bytes.
 *
 * @return quantity of bytes need to stored this information.
 */
int32_t CBaseSafeFile::sizeOf ( int32_t size ) 
{ 
    return size + sizeof(tcrc_t) + (flags & F_CSFILE_DYNAMIC?sizeof(int32_t):0);
}


/**
 * convert a simple read to multiple read (CDataArray)
 * @param offset offset in bytes from read data
 * @param data pointer to buffer where leave read data
 * @param len length in bytes of buffer, max length of data
 *
 * @return quantity of bytes read.
 */
int32_t CBaseSafeFile::read ( int64_t loff, void *data, int32_t dlen )
{
    CDataArray darray(1, data, dlen);
    return read(loff, 0, darray);
}

/**
 * convert a simple read to multiple read (CDataArray), this method allow to define flags in this read
 * @param offset offset in bytes from read data
 * @param flags flags used in this operation
 * @param data pointer to data
 * @param len length in bytes of buffer, max length of data
 *
 * @return quantity of bytes read.
 */
int32_t CBaseSafeFile::read ( int64_t loff, uint32_t flags, void *data, int32_t dlen )
{
    CDataArray darray(1, data, dlen);
    return read(loff, flags, darray);
}

/**
 * convert a simple write to multiple writes (CDataArray)
 * @param offset offset in bytes where store data
 * @param data pointer to data
 * @param len length in bytes of data
 *
 * @return quantity of bytes written.
 */
int32_t CBaseSafeFile::write ( int64_t loff, const void *data, int32_t dlen )
{
    return write(loff, 0, CDataArray(1, data, dlen));
}

/**
 * convert a simple write to multiple writes (CDataArray) with one dynamic data write, this method
 * allow to define flags in these writes
 * @param offset offset in bytes where store data
 * @param flags flags used in this operation
 * @param data pointer to data
 * @param len length in bytes of data
 *
 * @return quantity of bytes written.
 */
int32_t CBaseSafeFile::write ( int64_t loff, uint32_t flags, const void *data, int32_t dlen )
{
    return write(loff, flags, CDataArray(1, data, dlen));
}
    
/**
 * write to multiple writes (CDataArray), this method
 * allow to define flags in these writes
 * @param offset offset in bytes where store data
 * @param flags flags used in this operation
 * @param darray container with data to be stored
 *
 * @return quantity of bytes written.
 */
int32_t CBaseSafeFile::write ( int64_t loff, uint32_t _flags, const CDataArray &darray )
{
    int32_t len = 0;

    // add to current flags, the default flags.
    _flags |= flags;
    try
    {        
    	// Store information in continuous buffer data (workarea)
        if (_flags & F_CSFILE_DYNAMIC)
        {
        	C_LOG_DEBUG("writing F_CSFILE_DYNAMIC");
        	len = fillWorkArea(sizeof(int32_t), darray, 0);
        }
        else
        {
        	len = fillWorkArea(0, darray);
        }
             
        // write this data over file
        current_offset = writeWorkArea(loff, len, 0, &last_offset);
        C_LOG_TRACE("[%s] loff:%s flags:%02X len:%d current_offset:%lld last_offset:%lld", alias, offsetStr(loff), _flags, len, current_offset, last_offset);
		if (C_LOG_DEBUG_ENABLED) {
			if (len > 1024) { 
				C_LOG_DEBUG("[%s] data: %s...%s", alias, c_log_data2hex(work_area, 0, 256), c_log_data2hex(work_area, len + sizeof(tcrc_t) - 256, 256));
			}
			else {
				C_LOG_DEBUG("[%s] data: %s", alias, c_log_data2hex(work_area, 0, len + sizeof(tcrc_t)));
			}
		}
    }
    catch (CSafeFileException &e)
    {
        if (_flags & F_CSFILE_EXCEPTIONS) throw;
        return e.getErrorCode();
    }
    return len;
}

/**
 * Method to move current file pointer to top
 */
void CBaseSafeFile::goTop ( void )
{
    current_offset = 0;
    lseek64(fd, 0, SEEK_SET);    
}

/**
 * Method to multiple read of file, after this read this method verifies CRC of all data
 * @param loff offset from read data
 * @param _flags flags used in read, possible flags are: F_CSFILE_DYNAMIC (Length of records was variable),
 * 				 F_CSFILE_IGNORE_CRC (Ignore if CRC is valid),
 * 				 F_CSFILE_EMPTY_DATA (to allow produce error when CRC fails because is an empty record),
 * 				 F_CSFILE_EXCEPTIONS (When found errors produce an exception)
 * @param darray cointainer where store date read.
 *
 * @return length of bytes read with CRC, but not with dynamic size !!! BE CARREFUL
 */
const char *CBaseSafeFile::offsetStr ( int64_t loff )
{
	static char svalue[20];
	
	if (loff == C_CSFILE_ENDFILE) return "C_CSFILE_ENDFILE";
	if (loff == C_CSFILE_CURRPOS) return "C_CSFILE_CURRPOS";
	
	snprintf(svalue, sizeof(svalue), "%lld", loff);	
	return svalue;
}

int32_t CBaseSafeFile::read ( int64_t loff, uint32_t _flags, CDataArray  &darray )
{
    int32_t len = 0, vlen = 0;
    _flags |= flags;

    C_LOG_DEBUG("[%s] read(%lld, %08X, %s)", alias, loff, _flags, darray.debug().c_str());
    try
    {   
        bool valid = false;
        tcrc_t crc;
        //uint8_t *pdataSkip = work_area;
		int32_t pdataOffset = 0;
        int32_t skip = 0;
        
        // in case of dynamic read need to read length before
        if (_flags & F_CSFILE_DYNAMIC)
        {
            C_LOG_TRACE("[%s] _loff:%s current_offset:%lld", alias, offsetStr(loff), current_offset);

            // read in workarea length of next information
            loff = readWorkArea(loff, _flags, sizeof(int32_t), 0, &last_offset);
            vlen = ((int32_t *)work_area)[0];

            // in next reads must skip first 4 bytes because it was used to store
            // dynamic length of data
            skip = sizeof(int32_t);

            // increase pointer to workarea + 4
            pdataOffset += skip;
            C_LOG_TRACE("[%s] loff:%lld vlen:%d %08X", alias, loff, vlen, vlen);

            // to calculate length of data must subtract size of CRC (4 bytes), this information
            // was in bottom of data. This subtract was made to standard length, in two way if
            // is dynamic or static (see next else)
            len = vlen - sizeof(tcrc_t);
        }
        else len = darray.size();

        // read data in work area, use skip and add to length the size of CRC, in this
        // way read final CRC too.
        current_offset = readWorkArea(loff, _flags, len + sizeof(tcrc_t), skip);
        C_LOG_TRACE("[%s] loff:%s flags:%02X len:%d skip:%d", alias, offsetStr(loff), _flags, len, skip);
		if (C_LOG_DEBUG_ENABLED) {
			if (len > 1024) { 
				C_LOG_DEBUG("[%s] data: %s...%s", alias, c_log_data2hex(work_area, 0, 256), c_log_data2hex(work_area, len + sizeof(tcrc_t) - 256, 256));
			}
			else {
				C_LOG_DEBUG("[%s] data: %s", alias, c_log_data2hex(work_area, 0, len + sizeof(tcrc_t)));
			}
		}
        // calculate CRC of work area, in this case length don't consider CRC
        crc = crc32(work_area, len);
        tcrc_t dcrc = *((tcrc_t *)(work_area+len));

        // size of CRC is added to length
        // len += sizeof(tcrc_t);
        if (_flags & F_CSFILE_DYNAMIC)
        	len -= sizeof(int32_t);
        valid = (dcrc == crc);
        
        // in case of CRC not match
        if (!valid)
        {
        	// in case of all data read is zero (indicates with flag crc_zero_flag) and
        	// crc stored is zero too means that information was empty, never stored.
        	// Normaly this feature is used when records are fixed, and holes (records
        	// not stored) are allowed. If flag F_CSFILE_EMPTY_DATA is present produce
        	// an EMPTY_DATA error, otherwise could produce a CRC error.

			bool crc_zero_flag = memzero(work_area, len);
            if (crc_zero_flag && !dcrc && (_flags & F_CSFILE_EMPTY_DATA))
                throw CSafeFileException(__FILE__,__LINE__, E_CSFILE_EMPTY_DATA);

            // if flag F_CSFILE_IGNORE_CRC ignore this CRC fail, and continue, otherwise
            // generate an exception.
            if (!(_flags & F_CSFILE_IGNORE_CRC))
            {
                C_LOG_WARN("[%s] fails CRC dcrc:%08X crc:%08X crc_zero_flag:%d", alias, dcrc, crc, crc_zero_flag);
                throw CSafeFileException(__FILE__,__LINE__, E_CSFILE_CRC);
            }
        }

        // store information read in container, this data doesn't contain dynamic length.
        C_LOG_DEBUG("[%s] darray.setFromBuffer(%p, %d) [%s]", alias, work_area + pdataOffset, len, darray.debug().c_str() );
        darray.setFromBuffer(work_area + pdataOffset, len);
    }
    catch (CSafeFileException &e)
    {
        C_LOG_DEBUG("[%s] EXCEPTION: %s", alias, e.what());

        // in case of exception, check if need propagate this exception or must return
        // errorcode of this exception
        if (_flags & F_CSFILE_EXCEPTIONS) throw;
        else return e.getErrorCode(); 
    }
    return len;
}

/**
 * Method to fill work area (memory) with array of data, at end of this data add a CRC of all data
 * @param len bytes to skip in work area
 * @param darray class that contain a list of buffer or data
 * @param offset_len place where store len of all data, if this value is negative, the length of all
 *                   data doesn't stored.
 * @return length of information, this length includes CRC
 */
int32_t CBaseSafeFile::fillWorkArea ( int32_t len, const CDataArray &darray, int32_t offset_len )
{
    C_LOG_DEBUG("[%s] len=%d darray.size=%d", alias, len, darray.size());
	checkWorkAreaSize(darray.size() + len);
	
	if (darray.size() > (work_area_size-len)) {
		C_LOG_ERR("try to write data too big dlen:%d maxdlen:%d on [%s]", darray.size(), max_data_size, alias);
		throw CSafeFileException(__FILE__,__LINE__, E_CSFILE_DATA_TOO_BIG);
	}

    // write a array of data on working area, starting on len, function returns
    // final length
    uint8_t *cwork = work_area;
    cwork += len;
    
    C_LOG_DEBUG("[%s] len=%d darray.count=%d", alias, len, darray.count());

    // for each element of darray, copy data in work area (memory)
    for (int32_t index = 0; index < darray.count(); ++index )
    {
        C_LOG_DEBUG("cwork = work_area + %d/%d darray.data(%d)=%p darray.len(%d)=%d", (int)(cwork-work_area),work_area_size,index, darray.data(index), index, darray.len(index));
        memcpy(cwork, darray.data(index), darray.len(index));
        cwork += darray.len(index);
        C_LOG_DEBUG("len (%d) +=%d [%d]", len, darray.len(index), index);
        len += darray.len(index);
    }

    // store len of all data and CRC
    if (offset_len >= 0)
    {
        *((int32_t*)(work_area+offset_len)) = len + sizeof(tcrc_t);
        C_LOG_DEBUG("*** setting len as %d", len + sizeof(tcrc_t));
    }
    
/*	if (alignment > 0) {
		int32_t tlen = len + sizeof(tcrc_t);
		int32_t padding = alignment - (tlen % alignment);
		if ( padding != alignment ) {
			memset(cwork, 0, padding);
			len += padding;
			cwork += padding;
		}
	}*/
	
    // add CRC at end of data
    *((tcrc_t*)cwork) = crc32(work_area, len);
    len +=  sizeof(tcrc_t);
    return len;
}

/**
 * Method to write work area (memory) to file (disk)
 * @param loff offset where data was saved in file. This offset could be C_CSFILE_ENDFILE to write/append at end of file.
 * @param len bytes of data, included skipped bytes. Bytes stored was (len - skip).
 * @param skip bytes of work area (memory) that was skiped, if this parameter is greater than 0, len is reduced in skip bytes.
 * @param last_off output var where is stored real offset where data is stored
 *
 * @return last position after write, this position could be used in next sequencial write.
 */
int64_t CBaseSafeFile::writeWorkArea ( int64_t loff, int32_t len, int32_t skip, int64_t *last_loff )
{
    int64_t _loff;

	checkWorkAreaSize(len);

    // go to the position, and obtain real offset
    if (loff == C_CSFILE_ENDFILE) _loff = lseek64(fd, 0, SEEK_END);
    else _loff = lseek64(fd, loff, SEEK_SET);

	C_LOG_DEBUG("BACKTRACE:%s", strbacktrace(10));
    C_LOG_DEBUG("write_work_area (%s) loff:%lld%s [%lld] len:%d skip:%d %08X...%08X", alias,
    						_loff, (loff == C_CSFILE_ENDFILE?"*":""), _loff+(len-skip), (len-skip), skip,
    						*(uint32_t*)(work_area+skip),*(uint32_t*)(work_area+len-4));

    // if last_loff was specified, information is stored
    if (last_loff)
        *last_loff = _loff;

    // check real offset returned by lseek
    if (_loff < 0)
    {
    	C_LOG_ERR("invalid offset loff=%lld len=%d skip=%d errno=%d %s", loff, len, skip, errno, strerror(errno));
        throw CSafeFileException(__FILE__,__LINE__, E_CSFILE_WRITE_SEEK);
    }
            
    // substract of len skipped bytes and write information in file (disk)
    len -= skip;
    int32_t bytes = ::write(fd, work_area+skip, len);

    // how data stored in file change, need to reset this part of cache.
    clearCache(_loff, len);
    if (bytes != len)
    {
    	C_LOG_ERR("write fails fd=%d skip=%d len=%d, errno=%d %s", fd, skip, len, errno, strerror(errno));
        throw CSafeFileException(__FILE__,__LINE__, E_CSFILE_WRITE);
    }

    // check if this write was synchronous
    if (flags & F_CSFILE_AUTOSYNC)
        sync();

    return _loff + len;
}        


/**
 * Method to clear one part of file cache
 * @param loff offset of file that cache must be cleared
 * @param len bytes of data of file cache that must be cleared
 */
void CBaseSafeFile::clearCache ( int64_t _loff, int32_t len )
{
	// check if clear cache doesn't apply, cache is empty, or cache length is zero,
	// also if len = 0
	if (cache_offset < 0 || !cache_len)
        return;
        
	// in this function try to evaluate if cache must be erased, consider
	// segment D, segment that start on _loff with length len. To explain
	// take D1 as _loff and D2 as _loff + len - 1.
	//
	//                 D1                      D2
	//           cache_offset     (cache_offset + cache_len)
	//                 |                        |
	// ................|========================|.....................
	//                 |                        |
	// <-------A------>|<---------B------------>|<------ C ---------->
	//
	// if D1 is C, sure that segment D not have collision with cache,
	// because D2 > D1, to have collision D1 must be in A or B.
	//
	// if D2 is A, sure that segment D not have collision with cache,
	// because D1 < D2, to have collision D2 must be in B or C

	if ((_loff < (cache_offset + cache_len)) && ((_loff + len) > cache_offset))
	{
		cache_offset = -1;
        cache_len = 0;
        C_LOG_DEBUG("reset cache");
    }
}

/**
 * Method to make sync of file, if it's opened
 */
void CBaseSafeFile::sync ( void )
{
    if (fd>=0) {
		C_LOG_DEBUG("[%s] sync fd:%d", alias, fd);
        fdatasync(fd); 
	}
}

/**
 * Method to move current pointer of file to other position checking if arrive at end of file. Check
 * if exits enougth bytes. This method don't write or read anything.
 * @param loff offset where move
 * @param len bytes of data try to write or to red
 * @param write flag to indicate if will read or write. At moment this flags haven't real effect.
 *
 * @return position where start next read or write.
 */
int64_t CBaseSafeFile::seek ( int64_t loff, int32_t len, bool write )
{
    int64_t _loff;  

	if (fd < 0) {
        throw CSafeFileException(__FILE__,__LINE__, write ? E_CSFILE_WRITE_SEEK:E_CSFILE_READ_SEEK);
	}
    // obtain current offset and override this position in loff
    if (loff == C_CSFILE_CURRPOS)
        loff = lseek64(fd, current_offset, SEEK_SET);

    // go to end of file, and get this position in _loff
    _loff = lseek64(fd, 0, SEEK_END);
    C_LOG_DEBUG("[%s] seek loff:%lld _loff:%lld len:%d", alias, loff, _loff, len);

    // check if seek operation found errors
    if (_loff < 0)
    {
        throw CSafeFileException(__FILE__,__LINE__, write ? E_CSFILE_WRITE_SEEK:E_CSFILE_READ_SEEK);
    }

    // in case that you want to go to end of file, setting loff with end of file
    if ( loff == C_CSFILE_ENDFILE )
    {
    	loff = _loff;
    }

	// check if seek go out of dimensions of file
    if ( (loff+(int64_t)len) > _loff && !f_write_mode )
    {
        C_LOG_DEBUG("[%s] seek EOF flags = %08X (%lld,%d) > %lld", alias, flags, loff, len, _loff);
        throw CSafeFileException(__FILE__,__LINE__, E_CSFILE_EOF);
    }

    // if loff same than _loff (end of file), not need to seek, because previous lines
    // move to last position of file.
    if ( _loff != loff )
    {
    	_loff = lseek64(fd, loff, SEEK_SET);
    }

    // check if seek operation found errors
    if (_loff < 0)
    {
        throw CSafeFileException(__FILE__,__LINE__, write ? E_CSFILE_WRITE_SEEK:E_CSFILE_READ_SEEK);
    }
    return _loff;
}

void CBaseSafeFile::checkWorkAreaSize ( int32_t len )
{
	if (work_area_size >= (len + WORK_AREA_RESERV_SIZE)) return;
	
	int32_t pws = work_area_size;
	work_area_size = ((len + WORK_AREA_RESERV_SIZE)/delta_page + ((len + WORK_AREA_RESERV_SIZE) % delta_page ? 1 : 0)) * delta_page;
	
	C_LOG_TRACE("[%s] checkWorkAreaSize %d >= (%d + %d) --> %d", alias, pws, len, WORK_AREA_RESERV_SIZE, work_area_size );
	allocateWorkArea();
}

void CBaseSafeFile::allocateWorkArea ( void )
{
	// if workarea have memory allocated, before free it.
	if (work_area && work_area_size > 0) {
		work_area = (uint8_t *)realloc(work_area, work_area_size);
	}
	else if (work_area_size > 0) {
		work_area = (uint8_t *)malloc(work_area_size);
		memset(work_area, 0, work_area_size);
	}
}
/*
 * read in workarea (memory) information from file
 *
 * @param loff offset in bytes of data to be read
 * @param flags flags used in this operation, this flags added to default flags
 * @param len length of data to be read
 * @param skip offset in bytes of workarea to store data read
 * @param last_loff out parameter (optional) where is stored position where start read.
 */
int64_t CBaseSafeFile::readWorkArea ( int64_t loff, uint32_t flags, int32_t len, int32_t skip, int64_t *last_loff )
{
    C_LOG_WARN("[%s] loff:%s len:%d", alias, offsetStr(loff), len);
	
	checkWorkAreaSize(len);
    len -= skip;
    int64_t _loff = seek(loff, len);

    // if optional out parameter last_off was specified, stored in this offset where
    // start read operation
    if (last_loff)
        *last_loff = _loff;
    
    C_LOG_DEBUG("loff:%s _loff:%lld len:%d", offsetStr(loff), _loff, len);
    int32_t bytes;        

    if (cache_size > 0)
    {
        int32_t iloop = 0, clen = 0;
        
        // needs loop because read could be done in two steps, first reading from cache,
        // second reading from file second part.
        int32_t bytes = 1;
        while (len > 0)
        {    
            ++iloop;

            // check if at least a part data is in cache, if not enter in this if statement.
            if (cache_offset < 0 || _loff < cache_offset || _loff >= (cache_offset + cache_len))
            {
            	// setting the beginning of cache as beginning of current read
                cache_offset = _loff;
                cache_len = 0;
                C_LOG_DEBUG("[%s] cache miss cache_offset: %lld", alias, cache_offset);
                lseek64(fd , cache_offset, SEEK_SET);
                bytes = ::read(fd, cache, cache_size);
                if (bytes <= 0) {
					C_LOG_DEBUG("bytes: %d", bytes);
                    break;
				}
                cache_len = bytes;
                C_LOG_DEBUG("cache cache_offset: %lld cache_len: %ld", cache_offset, cache_len);
                continue;
            }            
            clen = cache_len - (_loff - cache_offset);
            if (clen > len) clen = len;
            C_LOG_DEBUG("clen:%d cache_len:%d _loff:%lld cache_offset:%lld (_loff - cache_offset):%lld",
            					clen, cache_len, _loff, cache_offset, (_loff - cache_offset));
            memcpy(work_area + skip, cache + (_loff - cache_offset), clen);
            skip += clen;
            _loff += clen;
            len -= clen;            
        } // end of loop
        if (len > 0 && bytes == 0)
        	throw CSafeFileException(__FILE__,__LINE__, E_CSFILE_EMPTY_DATA);
        if (len > 0)
            throw CSafeFileException(__FILE__,__LINE__, E_CSFILE_READ);
    }
    else // (cache_size <= 0)
    {
    	// in this case read direct over memory, without cache
        C_LOG_DEBUG("[%s] read(fd:%d, work_area:%p + skip:%d = %p, len:%d)", alias, fd, work_area, skip, work_area, len);
        bytes = ::read(fd, work_area+skip, len);

        if (bytes == 0)
        	throw CSafeFileException(__FILE__,__LINE__, E_CSFILE_EMPTY_DATA);
        if (bytes != len) {
            C_LOG_ERR("[%s] bytes(%d)!=len(%d)", alias, bytes, len);
            throw CSafeFileException(__FILE__,__LINE__, E_CSFILE_READ);
        }
        _loff += bytes;
    }
    return _loff;   
}        

void CBaseSafeFile::findLastSignatureReg ( int32_t max_size, uint32_t signature, uint32_t signature_mask, int32_t signature_offset )
{	
	off_t offset = ::lseek(fd, 0, SEEK_END);
	if (offset % alignment) {
		offset += (alignment - (offset % alignment));
	}
	
	if (max_size % alignment) {
		max_size = ((max_size / alignment) + 1) * alignment;
	}
	
	uint8_t *tmp = (uint8_t *)malloc(max_size);
	int32_t bytes;
	uint32_t *pSign;
	
	while (offset > 0) {
		offset = offset > max_size ? offset - max_size : 0;
		::lseek(fd, offset, SEEK_SET);
		bytes = ::read(fd, tmp, max_size);
		pSign = (uint32_t *)(tmp + signature_offset);
		while (bytes > (signature_offset + (int32_t)sizeof(uint32_t))) {
			if ((*pSign & signature_mask) == signature) {
				if (flags & F_CSFILE_DYNAMIC) {
					uint32_t *dlen = (uint32_t *)(((uint8_t *)pSign) - signature_offset);
					tcrc_t crc = crc32(dlen, *dlen);
					if (*(uint32_t *)((uint8_t *)dlen + *dlen) == crc) {
						printf("FOUND ON OFFSET %ld\n", offset + ((uint8_t *)dlen - tmp));
					}
				}
			}
			bytes -= (alignment > 0 ? alignment : 0);
		}
	}
}

