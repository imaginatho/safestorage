#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>

#include <iostream>
#include <sstream>
#include <string>
#include <exception>
using namespace std;

#include <c_log.h>
#include <safe_storage_imp.h>

static const char *__safe_storage_extensions [] = {"", ".idx", ".rlg", ".st"};
                           
#define CSTORAGE_SIGNATURE          0x51213298
#define CSTORAGE_SIGNATURE_MASK     0x00000007

#define DECLARE_STRUCT(X,Y)			X Y; memset(&Y, 0, sizeof(Y));
#define DECLARE_DEFAULT_STRUCT(X,Y)	X __##Y; if (!Y) { memset(&__##Y, 0, sizeof(__##Y)); Y=&__##Y; };

#define CSTORAGE_MODE_STANDARD	0
#define CSTORAGE_MODE_REBUILD	1

template <class R>
bool cmp_struct_field(R st1, R st2, const char *st1_name, const char *st2_name, const char *fieldname, int32_t pos )
{
	if (st1 == st2) return true;

	std::stringstream msg;
	msg << "Field " << fieldname << "of structures " << st1_name;

	if (pos >= 0) {
		msg << "[" << pos << "]";
	}

	msg << "(" << st1 << ") and " << st2_name;

	if (pos >= 0) {
		msg << "[" << pos << "]";
	}

	msg << "(" << st2 << ") are different";
	C_LOG_ERR(msg.str().c_str());

	return false;
}

#define CMP_STRUCT_FIELD(ST1,ST2,FIELD,R)\
	R |= cmp_struct_field<__typeof__(ST1.FIELD)>(ST1.FIELD,ST2.FIELD,#ST1,#ST2,#FIELD,-1);


#define CMP_STRUCT_ARRAY_FIELD(ST1,ST2,FIELD,R)\
	for (int32_t __##FIELD##__index__=0; __##FIELD##__index__ < (int32_t)(sizeof(ST1.FIELD)/sizeof(ST1.FIELD[0])); ++__##FIELD##__index__ ) {\
		R |= cmp_struct_field<__typeof__(ST1.FIELD[0])>(\
				ST1.FIELD[__##FIELD##__index__],\
				ST2.FIELD[__##FIELD##__index__],\
				#ST1,#ST2,#FIELD,__##FIELD##__index__);\
	}\

typedef union 
{
    CSafeStorageIndexReg index;
    CSafeStorageDataReg data;
    CSafeStorageLogReg log;
    uint32_t hash_key;    
} CSafeStorageHashReg;


/*
 * Constructor class CSafeStorage, this method initialize structures, and create list of
 * safe files managed with object. These files are added in list files.
 */
CSafeStorage::CSafeStorage ( void )
    :	findex(0, D_CSTORAGE_INDEX_CACHE), 
		flog(0, D_CSTORAGE_LOG_CACHE), 
		fdata(0,0, D_CSTORAGE_DATA_INIT_SIZE, D_CSTORAGE_DATA_DELTA_SIZE /*, 16*/), 
		fstate(0,0)
{
	cursor = -1;
	slot = 0;
	rdwr = false;
	mode = CSTORAGE_MODE_STANDARD;
    memset(&state, 0, sizeof(state));
    files.push_back(&fdata);
    files.push_back(&findex);
    files.push_back(&flog);
    files.push_back(&fstate);    
}
    
/*
 * Destructor, that close all files.
 */
CSafeStorage::~CSafeStorage ()
{
    close();
}

/*
 * Method that close all file, using list file, close each file.
 */
int32_t CSafeStorage::close ( uint32_t flags )
{
    CBaseSafeFileList::iterator it = files.begin();
    
	saveState(true);
    while (it != files.end())
    {
        (*it)->close();
        ++it;
    }
	dirtySerials.clear();
	
	if (rdwr) sync(true);

	fauto_commit = false;
	return E_CSTORAGE_OK;
}

/*
 * Open a new environment, a list of files in one directory.
 *
 * @param filename prefix, filename without extension
 * @param flags flags that applies to this operation. Current, no flags defined for this operation.
 *
 * @return 0 if no errors, error code if error occurs.
 */
int32_t CSafeStorage::open ( const string &filename, uint32_t flags, uint32_t hash_key )
{
    string _filename;
    fauto_commit = false;
    try
    {
        flags |= F_CSFILE_EXCEPTIONS;
        int32_t index = 0;

    	// Check if all file exists
		string basename = normalizeFilename(filename);
		rdwr = (flags & F_CSTORAGE_WR);
		int32_t count = checkFilesPresent(basename, (flags & F_CSTORAGE_WR) ? (R_OK|W_OK):R_OK);
        if (count > 0) {
			if (count == (int32_t)files.size() && (flags & F_CSTORAGE_CREATE)) return create(basename, flags, hash_key);
			return E_CSTORAGE_OPEN_FILE_NO_ACCESS;
		}

        CBaseSafeFileList::iterator it = files.begin();
        // for each file open file
        while (it != files.end())
        {
            _filename = basename + __safe_storage_extensions[index++];
            (*it)->open(_filename, flags);
            ++it;
        }

		loadState();
        // check if hash of all files it's the same.
        int32_t result = checkHashKey();
        if (result != E_CSTORAGE_OK)
        	throw CSafeFileException(__FILE__, __LINE__, result);

        if (flags & F_CSTORAGE_AUTO_COMMIT) {
			C_LOG_INFO("Setting autocommit on (flgs:%08X)", flags);
        	fauto_commit = true;
		}

    }
    catch (const CSafeFileException &e)
    {
    	// if error found, all files must be closed
        C_LOG_ERR("e.getErrorCode()=%d", e.getErrorCode());
        close();
        return e.getErrorCode(); 
    }
    return E_CSTORAGE_OK;
}

int32_t CSafeStorage::setFlags ( uint32_t flags )
{
	string _filename;
	CBaseSafeFileList::iterator it = files.begin();
	for (int _loop = 0; _loop < 2; ++_loop) {
		while (it != files.end())
		{
			_filename = (*it)->getFilename();
			// _filename += __safe_storage_extensions[index++];
			int result = (*it)->setFlags(flags | (_loop == 0 ? F_CSFILE_CHECK_FLAGS : 0));
			if (_loop == 0 && result < 0) {
				return E_CSTORAGE_ERROR;
			}
			++it;
		}
	}
	return 0;
}

string CSafeStorage::normalizeFilename ( const string &filename )
{
	int extlen = strlen(__safe_storage_extensions[0]);
	if (filename.compare(filename.size() - extlen, extlen, __safe_storage_extensions[0]) == 0) {
		return filename.substr(0, filename.size() - extlen);
	}
	return filename;
}

/*
 * Method to create all files
 *
 * @param filename prefix, filename without extension
 * @param flags flags that applies to this operation. Current, no flags defined for this operation.
 *
 * @return 0 if no errors, error code if error occurs.
 */
int32_t CSafeStorage::create ( const string &filename, uint32_t flags, uint32_t hash_key )
{
   	C_LOG_DEBUG("create(%s, %04X)", filename.c_str(), flags);

	fauto_commit = false;
	string basename = normalizeFilename(filename);
	if (mode != CSTORAGE_MODE_REBUILD) {
		// Check if any file is created
		if (checkAnyFilePresent(basename)) {
			C_LOG_ERR("FileExists");
			return E_CSTORAGE_CREATE_FILE_EXISTS;
		}
	}
    string _filename;
    try
    {
        CBaseSafeFileList::iterator it = files.begin();
        int32_t index = 0;
        
		uint32_t nflags = flags | (F_CSFILE_EXCEPTIONS|F_CSFILE_CREATE|F_CSFILE_WR);
        // list of files is created on constructor, array _safe_storage_extensions
        // contains extension for each type of file
        while (it != files.end())
        {
            _filename = basename + __safe_storage_extensions[index++];
			(*it)->open(_filename, ((*it) == &fdata && (mode == CSTORAGE_MODE_REBUILD)) ? flags : nflags);
            ++it;
        }

        // to link all files and avoid that one of them was replaced, renamed, etc.. generate
        // a hash key and this key is stored in all files.
		if (mode == CSTORAGE_MODE_REBUILD) {
			getHashKey(hash_key);
		}
		else if ((flags & F_CSTORAGE_SET_HASH_KEY) == 0) {
			hash_key = generateHashKey();
		}
        writeHashKey(hash_key, flags);

        if (flags & F_CSTORAGE_AUTO_COMMIT) {
			C_LOG_INFO("Setting autocommit on (flgs:%08X)", flags);
        	fauto_commit = true;
		}
    }
    catch (CSafeFileException &e)
    {
    	// if error found, all files must be closed
        C_LOG_ERR("CSafeFileException %d on %s:%d", e.code(), e.file().c_str(), e.line());
    	close();
        return e.getErrorCode(); 
    }
    return 0;
}


int32_t CSafeStorage::rebuild ( const string &filename, uint32_t flags )
{
    DECLARE_STRUCT(CSafeStorageDataReg, r_data)
	// donde tenemos guardaa
   	C_LOG_DEBUG("rebuild(%s, %04X)", filename.c_str(), flags);

	mode = CSTORAGE_MODE_REBUILD;
	int32_t result = CSafeStorage::create(filename, flags);
	if (result != E_CSTORAGE_OK) {
		return result;
	}
	
    DECLARE_STRUCT(CSafeStorageState, _state)
    DECLARE_STRUCT(CSafeStorageHashReg, h)

	uint8_t *data = (uint8_t *)malloc(1024*1024);
	result = fdata.read(0, h.data);
    if (result < 0)
    {
    	C_LOG_ERR("Internal error %d reading data 0", result);
    	return E_CSTORAGE_DATA_READ;
    }
	// TODO: Rebuild, gestion del rebuild
    while ((result = fdata.read(C_CSFILE_CURRPOS, r_data, data, 1024*1024)) > 0)
//    while ((result = fdata.read(C_CSFILE_CURRPOS, r_data, NULL, 0)) > 0)
    {
		printf("%08X %8d %8d %02d %6d %02X\n", r_data.signature, r_data.serial, r_data.sequence, r_data.slot, r_data.len, r_data.flags);
		writeFiles(r_data, data, r_data.len);
    }
	printf("end rebuild (%d)\n", result);
	mode = CSTORAGE_MODE_STANDARD;
    saveState(false);
    sync(true);
	return result;
}

/*
 * Check if any file is accessible
 *
 * @param filename prefix of files, name without extension
 * @param mode mode for access check.
 *
 * @return number of files accessible. If returns 0 means files no exists.
 */
int32_t CSafeStorage::checkFilesPresent ( const string &filename, int32_t mode )
{
    string _filename;
    int32_t index = 0;
    int32_t result = 0;
    
    // for each file
    while (index < (int32_t)files.size())
    {
    	// generate filename of each file
        _filename = filename + __safe_storage_extensions[index++];

        // if file is accessible increment counter result.
        if (access(_filename.c_str(), mode)!= 0)
        {
        	C_LOG_DEBUG("checking file %s (not found)", _filename.c_str());
            ++result;
        }
        else {
        	C_LOG_DEBUG("checking file %s (found)", _filename.c_str());

        }
    }
    C_LOG_DEBUG("result=%d", result);
    return result;
}

int32_t CSafeStorage::checkAnyFilePresent ( const string &filename )
{
	return (files.size() - checkFilesPresent(filename, R_OK));
}

/*
 * Sync of all files
 */
void CSafeStorage::sync ( bool full )
{
	if (mode == CSTORAGE_MODE_REBUILD) return;
    fdata.sync();
	
	if (!full) return;
    findex.sync();
    flog.sync();
    fstate.sync();
}

/*
 * Method to make a commit of all made from last commit or rollback. This method
 * add commit as data (to recover), as commit as log, and updated status
 *
 * @return 0 means Ok, otherwise means error.
 */
int32_t CSafeStorage::commit ( void )
{
    DECLARE_STRUCT(CSafeStorageDataReg,r_data)
    
    if (fauto_commit)
    {
    	C_LOG_DEBUG("Ignore commit because we are in auto-commit mode");
    	return E_CSTORAGE_OK;
    }

    C_LOG_DEBUG("CSafeStorage::commit()");

    r_data.signature = CSTORAGE_SIGNATURE | T_CSTORAGE_COMMIT;
    r_data.sequence = state.last_sequence + 1;
	r_data.slot = slot;
    int32_t result = writeFiles(r_data);
	
	// TO-DO: open transactions, make a rollback?

	// clearDirtySerials();
	sync();
    return result;
}

void CSafeStorage::clearDirtySerials ( void )
{
	CSafeStorageSerialList::iterator it2, it = dirtySerials.begin();
	while (it != dirtySerials.end())
	{	
		it2 = it;
		++it;
		if ((*it2).second == slot) {
			dirtySerials.erase(it2);
		}
	}
}

/*
 * Method to make a rollback of all write made from last commit or rollback. This method make following steps:
 * 1) add rollback (begin) as data
 * 2) add rollback (begin) as log
 * 3) save state and synchronize all data, log, and state
 * 4) update all index of rollback, setting offset -1.
 * 5) synchronize index
 * 6) add rollback (end) as data linked to last index updated
 * 7) add rollback (end) as log linked to last index updated
 * 8) save state and synchronize all data, log, and state
 *
 * @return 0 means Ok, otherwise means error.
 */
int32_t CSafeStorage::rollback ( void )
{
    C_LOG_INFO("rollback");
    DECLARE_STRUCT(CSafeStorageDataReg,r_data)
    int32_t result = E_CSTORAGE_OK;

    if (fauto_commit)
    {
        C_LOG_DEBUG("Rollback disabled in auto-commit mode");
    	return E_CSTORAGE_NO_ROLLBACK_IN_AUTOCOMMIT;
    }

    C_LOG_DEBUG("CSafeStorage::rollback()");
    r_data.signature = CSTORAGE_SIGNATURE | T_CSTORAGE_ROLLBACK_BEGIN;
    r_data.sequence = state.last_sequence + 1;
	r_data.slot = slot;

    int32_t dlen = (dirtySerials.size()+1)* sizeof(tserial_t);
    tserial_t *serials = (tserial_t *)malloc(dlen);
    serials[0] = dirtySerials.size();

	CSafeStorageSerialList::iterator it = dirtySerials.begin();
	int32_t index = 1;
	while (it != dirtySerials.end())
	{
		if ((*it).second == slot) {
			serials[index++] = (*it).first;
		}
		++it;
	}

    result = writeFiles(r_data, serials, dlen);
    free(serials);

    // asociated to rollback_end stored last index modified, todo a check integrity
	// beetween index and data.

    memset(&r_data, 0, sizeof(r_data));
	r_data.sequence = state.last_sequence + 1;
	r_data.signature = CSTORAGE_SIGNATURE | T_CSTORAGE_ROLLBACK_END;
	r_data.slot = slot;

    result = writeFiles(r_data);
	clearDirtySerials();
	return result;

}

/*
 * Method used to verify content of data, recalculate all information and check it with index
 * state, and log files
 *
 * @return 0 means verify was ok, error in otherwise.
 */
int32_t CSafeStorage::verify ( uint32_t flags )
{
    DECLARE_STRUCT(CSafeStorageDataReg, r_data)
    DECLARE_STRUCT(CSafeStorageState, _state)

    int32_t result = fdata.read(0, r_data);
    if (result > 0)
    {
    	C_LOG_ERR("Internal error %d reading data 0", result);
    	return E_CSTORAGE_DATA_READ;
    }

    while ((result = fdata.read(C_CSFILE_CURRPOS, r_data)) > 0)
    {
        uint32_t type = r_data.signature & CSTORAGE_SIGNATURE_MASK;
        switch(type)
        {
//        	_state.last_offset_index;
//        	_state.last_offsets;
    		case T_CSTORAGE_COMMIT:
    		{
    			_state.last_close_sequence = r_data.sequence;
    			_state.last_close_offset = fdata.lastOffset();
    			_state.last_commit_sequences[r_data.slot] = r_data.sequence;
    			++_state.commit_count;
    			break;
    		}
    		case T_CSTORAGE_WRITE:
    		{
    			++_state.data_count;
    			break;
    		}
    		case T_CSTORAGE_ROLLBACK_BEGIN:
    		{
    			++_state.rollback_begin_count;
    			break;
    		}
    		case T_CSTORAGE_ROLLBACK_END:
    		{

    			_state.last_close_sequence = r_data.sequence;
    			_state.last_close_offset = fdata.lastOffset();
    			++_state.rollback_end_count;
    			break;
    		}
    		case T_CSTORAGE_STATUS:
    		{
    			break;
    		}
    		default:
    		{
    			C_LOG_ERR("Internal error: Non expected data type (0x%08X) was found", type);
    			return E_CSTORAGE_DATA_READ;
    		}
        }
        _state.last_sequence = r_data.sequence;
		++_state.count;
    }

    CMP_STRUCT_FIELD(state,_state,count,result);
    CMP_STRUCT_FIELD(state,_state,data_count,result);
    CMP_STRUCT_FIELD(state,_state,commit_count,result);
    CMP_STRUCT_FIELD(state,_state,rollback_begin_count,result);
    CMP_STRUCT_FIELD(state,_state,rollback_end_count,result);
    CMP_STRUCT_FIELD(state,_state,last_offset_index,result);
    CMP_STRUCT_ARRAY_FIELD(state,_state,last_commit_sequences,result);
    CMP_STRUCT_ARRAY_FIELD(state,_state,last_rollback_sequences,result);
    CMP_STRUCT_FIELD(state,_state,last_close_sequence,result);
    CMP_STRUCT_FIELD(state,_state,last_sequence,result);
    CMP_STRUCT_ARRAY_FIELD(state,_state,last_offsets,result);


    return result;
}

int32_t CSafeStorage::recoverDirtySerials ( void )
{
    DECLARE_STRUCT(CSafeStorageDataReg ,r_data)

	uint64_t offset = state.last_close_offset;
    dirtySerials.clear();

    int32_t result = fdata.read(offset, r_data);
    if (result > 0)
    {
    	C_LOG_ERR("Internal error %d reading data on offset %lld", result, offset);
    	return E_CSTORAGE_DATA_READ;
    }

    uint32_t type = r_data.signature & CSTORAGE_SIGNATURE_MASK;
    if (type != T_CSTORAGE_COMMIT && type != T_CSTORAGE_ROLLBACK_BEGIN && type != T_CSTORAGE_ROLLBACK_END)
    {
    	C_LOG_ERR("Internal error: On offset %llld non expected data type (0x%08X) was found", offset, type);
    	return E_CSTORAGE_DATA_READ;
    }

    while ((result = fdata.read(C_CSFILE_CURRPOS, r_data)) > 0)
    {
        type = r_data.signature & CSTORAGE_SIGNATURE_MASK;
        if (type != T_CSTORAGE_WRITE)
        {
        	C_LOG_ERR("Internal error: Non expected data type (0x%08X) was found", offset, type);
        	return E_CSTORAGE_DATA_READ;
        }
        if ((r_data.flags & F_CSTORAGE_DF_AUTO_COMMIT) == 0)
        	dirtySerials[r_data.serial] = r_data.slot;
    }
    return result;
}

int32_t CSafeStorage::read ( tserial_t &serial, void *data, uint32_t dlen, uint32_t flags )
{
    C_LOG_DEBUG("read(%ld, %p, %d, %08X)", serial, data, dlen, flags);
    DECLARE_STRUCT(CSafeStorageIndexReg ,r_index)
    DECLARE_STRUCT(CSafeStorageDataReg ,r_data)
	cursor = serial;
    
    if (!rdwr) loadState();
    int32_t result = findex.readIndex(serial, r_index);
//    printf(">>>>>> %d %d %lld\n", serial, result, r_index.offset);

    if (result == E_CSFILE_EMPTY_DATA)
    	return E_CSTORAGE_SERIAL_NOT_FOUND;

    if (result < 0)
        return result;

	/* verify if offset error, or offset not initialized (=0) or deleted (=-1) */
    last_offset = getLastOffset(r_index, serial, flags);
    if (last_offset <= 0)
    	return E_CSTORAGE_SERIAL_NOT_FOUND;

    C_LOG_DEBUG("try to read %d bytes on offset %lld", dlen, last_offset);
    result = fdata.read(last_offset, r_data, data, dlen);
    if (result < 0)
		return result;
		
	if (C_LOG_DEBUG_ENABLED) {
		if (result > 1024) { C_LOG_DEBUG("data(%p): %s...%s", data, c_log_data2hex(data, 0, 256), c_log_data2hex(data, result - 256, 256)); }
		else { C_LOG_DEBUG("data(%p): %s",  data, c_log_data2hex(data, 0, result)); };
	}	
	
		
	result = result - sizeof(r_data);
	
    C_LOG_DEBUG("state.last_commit_sequences[%d]:%d r_data.sequence:%d autocommit:%s", r_data.slot, state.last_commit_sequences[slot], r_data.sequence,
		(r_data.flags & F_CSTORAGE_DF_AUTO_COMMIT)? "true":"false");

	if (C_LOG_DEBUG_ENABLED) {
		if (result > 1024) { C_LOG_DEBUG("data(%p): %s...%s", data, c_log_data2hex(data, 0, 256), c_log_data2hex(data, result - 256, 256)); }
		else { C_LOG_DEBUG("data(%p): %s",  data, c_log_data2hex(data, 0, result)); };
	}	
	
	return result;
}

int32_t CSafeStorage::readLog ( tseq_t seq, CSafeStorageDataReg &r_data, void *data, uint32_t dlen, uint32_t flags )
{
    DECLARE_STRUCT(CSafeStorageLogReg ,r_log)

    if (!rdwr) loadState();
    int32_t result = flog.readIndex(seq, r_log);
    if (result < 0)	return result;

    if (result == E_CSFILE_EMPTY_DATA)
    	return E_CSTORAGE_SEQUENCE_NOT_FOUND;

    if (result < 0)
        return result;

    int64_t offset = r_log.offset;
    if (offset < 0)
    	return E_CSTORAGE_SEQUENCE_NOT_FOUND;

    result = fdata.read(offset, r_data, data, dlen);
    if (result < 0)
        return result;

    return result - sizeof(r_data);
}

/*
 * Method to access to log
 *
 * @param seq sequence of log to read
 * @param serial out parameter, if it isn't null was stored serial number of this sequence
 * @param type out parameter, if it isn't null was stored type of action of this sequence
 * (write, commit, rollback-begin, rollback-end)
 *
 * @return 0 Ok, error in otherwise.
 */
int32_t CSafeStorage::readLogReg ( tseq_t seq, tserial_t &serial, uint8_t &type, uint32_t flags )
{
    DECLARE_STRUCT(CSafeStorageLogReg ,r_log)

    int32_t result = flog.readIndex(seq, r_log);
    if (result < 0)	return result;

    serial = r_log.serial;
    type = r_log.type;

    return 0;
}


/*
 * Write a hash (mark) in all files, to link them, to avoid that
 * one of files was replaced with another.
 */
int32_t CSafeStorage::writeHashKey ( uint32_t hash_key, uint32_t flags )
{
    DECLARE_STRUCT(CSafeStorageHashReg, h)
    
    C_LOG_TRACE("writeHashKey %08X", hash_key);
	
    // store hash in structure
    h.hash_key = hash_key;

    // write hash in files
	if (mode != CSTORAGE_MODE_REBUILD) {
		fdata.write(0, h.data);
	}
    flog.write(0, h.log);
    findex.write(0, h.index);

    // set hash in state structure, and save it.
    state.hash_key = hash_key;
    saveState(false);
    sync();
    return 0;
}

/*
 * Check that all files are liked, when files was created an random hash of 4 bytes was generated
 * and copy in all files. With this feature it's possible detect when files was replace by other
 * incorrect or old file. Data hash is considered how master hash. This information about hash is
 * stored in first record.
 *
 * @return if checks is passed return 0, if not return value that was an OR of all hash that fails.
 */
int32_t CSafeStorage::checkHashKey ( void )
{
    DECLARE_STRUCT(CSafeStorageHashReg, h)
	
    uint32_t hash_key;
    int32_t result = E_CSTORAGE_OK;
    
    fdata.read(0, h.data);
    hash_key = h.hash_key;
    C_LOG_DEBUG("CheckHashKey %08X", hash_key);

    flog.read(0, h.log);
    if (h.hash_key != hash_key) {
    	C_LOG_ERR("CheckHashKey Log [FAILS] (Data: %08X / Log: %08X)", hash_key, h.hash_key);
        result |= E_CSTORAGE_FAIL_HK_LOG;
    }
        
    findex.read(0, h.index);
    if (h.hash_key != hash_key) {
    	C_LOG_ERR("CheckHashKey Index [FAILS] (Data: %08X / Index: %08X)", hash_key, h.hash_key);
        result |= E_CSTORAGE_FAIL_HK_INDEX;
    }

    if (!rdwr) loadState();
    if (state.hash_key != hash_key) {
    	C_LOG_ERR("CheckHashKey State [FAILS] (Data: %08X / State: %08X)", hash_key, state.hash_key);
        result |= E_CSTORAGE_FAIL_HK_STATE;
    }
        

    C_LOG_DEBUG("CheckHashKey [%s] result = %08X", result == E_CSTORAGE_OK ? "OK":"FAILS", result);
    return result;
}

int32_t CSafeStorage::write ( tserial_t serial, const void *data, uint32_t dlen, uint32_t flags )
{	
    DECLARE_STRUCT(CSafeStorageDataReg ,r_data)
    
    C_LOG_DEBUG("CSafeStorage::write(%d, %p, %d) lseq:%d", serial, data, dlen, state.last_sequence);

/*	CSafeStorageSerialList::iterator it = dirtySerials.find(serial);
	if (it != dirtySerials.end() && (*it).second != slot) {
		C_LOG_DEBUG("Locked Register %d", serial);
		return E_CSTORAGE_LOCKED_REGISTER;
	}*/
	
	// TO-DO: cuando se usen los slots verificar que no está usado por otro slot.
    // prepare a data register of write
	r_data.signature = CSTORAGE_SIGNATURE | T_CSTORAGE_WRITE;
	r_data.serial = serial;
	r_data.sequence = state.last_sequence + 1;
	r_data.slot = slot;
	r_data.len = dlen;
	if (fauto_commit)
		r_data.flags |= F_CSTORAGE_DF_AUTO_COMMIT;
    
    return writeFiles(r_data, data, dlen);
}

int32_t CSafeStorage::applyLog ( CSafeStorageDataReg &r_data, const void *data, uint32_t dlen, uint32_t flags )
{
	return writeFiles(r_data, data, dlen);
}

int32_t CSafeStorage::writeFiles ( CSafeStorageDataReg &r_data, const void *data, uint32_t dlen )
{
    DECLARE_STRUCT(CSafeStorageIndexReg ,r_index)
    DECLARE_STRUCT(CSafeStorageLogReg ,r_log)	
	
    int32_t result = E_CSTORAGE_OK;

    int32_t type = r_data.signature & CSTORAGE_SIGNATURE_MASK;

	++state.count;
    state.last_sequence = r_data.sequence;
	r_log.serial = r_data.serial;
	r_log.slot = r_data.slot;
	r_log.type = type;
	C_LOG_DEBUG("type:%08X dlen:%d", type, dlen);
	switch (type)
	{
		case T_CSTORAGE_WRITE:
		{
			// prepare log register related to write
			r_log.offset = writeData(r_data, data, dlen);
			++state.data_count;

			// prepare index related to write
			setOldestOffset(r_index, r_data.serial, r_log.offset, r_data.sequence, r_data.flags, findex.readIndex(r_data.serial, r_index) < 0);

			// if error in data write, abort this write and returns error
			if (r_log.offset < 0LL) {
				return -1;
			}

/*			if (!fauto_commit)
				dirtySerials[r_data.serial] = r_data.slot;*/

			// save state with information
			saveState();

			flog.writeIndex(r_data.sequence, r_log);
			findex.writeIndex(r_data.serial, r_index);
			result = dlen;
			break;
		}

		case T_CSTORAGE_ROLLBACK_BEGIN:
		{
			writeData(r_data, data, dlen);
		
			writeSyncStateLogSync(r_log);

			// mark offset of all indexes as -1
		    tserial_t *serials = (tserial_t *) data;
		    int32_t count = serials[0];

		    // must: dlen / sizeof(tserial_t) == serials[0]+1

			int32_t index = 1;
			while (index <= count)
			{
				findex.readIndex(serials[index], r_index);
				setNewestOffset(r_index, serials[index], -1, 0, 0);
				int32_t result = findex.writeIndex(serials[index], r_index);
				if (result < 0)
					C_LOG_ERR("CSafeStorage::rollBack Error saving index %d of rollback", serials[index]);
				++index;
			}

			if (count > 0 && mode != CSTORAGE_MODE_REBUILD)
			{
				findex.sync();
			}
			break;
		}

		case T_CSTORAGE_ROLLBACK_END:
		{			
			writeData(r_data);
			
			writeSyncStateLogSync(r_log);

			break;
		}
		case T_CSTORAGE_COMMIT:
		{
			r_log.offset = writeData(r_data);

			++state.commit_count;
			state.last_commit_sequences[r_data.slot] = r_data.sequence;
			writeSyncStateLogSync(r_log);

			break;
		}
	}
	
	// check if store a status snapshot
	if (state.count % C_STORAGE_STATUS_FREQ == (C_STORAGE_STATUS_FREQ - 1)) {
		DECLARE_STRUCT(CSafeStorageDataReg,r_state_data)
    
		r_state_data.signature = CSTORAGE_SIGNATURE | T_CSTORAGE_STATUS;
		r_state_data.sequence = state.last_sequence + 1;
		r_state_data.slot = r_data.slot;

		memset(&r_log, 0, sizeof(r_log));
		++state.count;
		state.last_sequence = r_state_data.sequence;
		r_log.serial = r_state_data.serial;
		r_log.slot = r_state_data.slot;
		r_log.type = T_CSTORAGE_STATUS;
		r_log.offset = writeData(r_state_data);
		flog.writeIndex(state.last_sequence, r_log);
		saveState();
	}
	return result;
}

void CSafeStorage::writeSyncStateLogSync ( CSafeStorageLogReg &r_log )
{
	flog.writeIndex(state.last_sequence, r_log);
/*
	if (mode != CSTORAGE_MODE_REBUILD) fdata.sync();
	saveState();
	if (mode != CSTORAGE_MODE_REBUILD) flog.sync();*/
}

void CSafeStorage::saveState ( bool sync )
{
	if (mode == CSTORAGE_MODE_REBUILD || !rdwr) return;
    fstate.write(0, state);
/*    if (sync)
        fstate.sync();*/
}

void CSafeStorage::loadState ( void)
{
    fstate.read(0, state);
}

uint32_t CSafeStorage::generateHashKey ( void )
{
    int32_t fd;
    uint32_t result = 0;
    
    fd = ::open("/dev/random", O_RDONLY);
    if (fd >= 0)
    {
        if (::read(fd, &result, sizeof(result)) != sizeof(result))
            fd = -1;
        ::close(fd);
    }
    if (fd < 0) {
        srand ( time(NULL) );
        result = rand();
    }
    return result;
}

int32_t CSafeStorage::getState ( CSafeStorageState &state )
{
	state = this->state;
	return E_CSTORAGE_OK;
}

int32_t CSafeStorage::getHashKey ( uint32_t &hash_key )
{
    DECLARE_STRUCT(CSafeStorageHashReg, h)
	
    int32_t result = fdata.read(0, h.data);
	if (result == sizeof(h.data)) {
		result = E_CSTORAGE_OK;
		hash_key = h.hash_key;
	}
	return result;
}

void CSafeStorage::dumpState ( void )
{
    int32_t index;
/*    uint32_t hash_key;
    uint32_t last_updated;
    int32_t count;
    int32_t commit_count;
    int32_t rollback_begin_count;
    int32_t rollback_end_count;
    int32_t data_count;
    int8_t last_offset_index;

    tseq_t last_commit_sequences[MAX_SEQUENCE_SLOT];
    tseq_t last_rollback_sequences[MAX_SEQUENCE_SLOT];
    tseq_t last_close_sequence;
    tseq_t last_sequence;

    uint64_t last_close_offset;
    uint64_t last_offsets[N_SS_LAST_OFFSETS];
} __attribute__ ((packed));
*/    
    printf("[state]\n");
    printf("hash_key: %08X\n", state.hash_key);
    printf("last_updated: %d\n", state.last_updated);
    printf("count: %d\n", state.count);
    printf("commit_count: %d\n", state.commit_count);
    printf("rollback_begin_count: %d\n", state.rollback_begin_count);
    printf("rollback_end_count: %d\n", state.rollback_end_count);
    printf("last_offset_index: %d\n", state.last_offset_index);
	printf("last_commit_sequences[%u]:", sizeof(state.last_commit_sequences)/sizeof(state.last_commit_sequences[0]));
	for (index = 0; index < (int32_t)(sizeof(state.last_commit_sequences)/sizeof(state.last_commit_sequences[0])); ++index) {
		printf("%s%d", index?",":"", state.last_commit_sequences[index]);
	}
	printf("\n");
	printf("last_rollback_sequences[%u]:", sizeof(state.last_rollback_sequences)/sizeof(state.last_rollback_sequences[0]));
	for (index = 0; index < (int32_t)(sizeof(state.last_rollback_sequences)/sizeof(state.last_rollback_sequences[0])); ++index) {
		printf("%s%d", index?",":"", state.last_rollback_sequences[index]);
	}
	printf("\n");
    printf("last_close_sequence: %d\n", state.last_close_sequence);
    printf("last_sequence: %d\n", state.last_sequence);
    printf("last_close_offset: %lld\n", state.last_close_offset);
	printf("last_offsets[%d]:", sizeof(state.last_offsets)/sizeof(state.last_offsets[0]));
	for (index = 0; index < (int32_t)(sizeof(state.last_offsets)/sizeof(state.last_offsets[0])); ++index) {
		printf("%s%lld", index?",":"", state.last_offsets[index]);
	}
	printf("\n");
}

int64_t CSafeStorage::writeData ( CSafeStorageDataReg &r_data, const void *data, uint32_t dlen )
{
	if (mode != CSTORAGE_MODE_REBUILD) {
		int64_t result = fdata.write(r_data, data, dlen);
		if (result < 0) {
			C_LOG_ERR("Error writting data (%p,%p,%d) result %d", &r_data, data, dlen, (int32_t)result);
			return result;
		}
	}
	state.last_offset_index = (state.last_offset_index + 1) % (sizeof(state.last_offsets)/sizeof(state.last_offsets[0]));
	state.last_offsets[state.last_offset_index] = fdata.lastOffset();
	return state.last_offsets[state.last_offset_index];
}

int32_t CSafeStorage::removeFiles ( const string &filename )
{
    string _filename;
    int32_t index = 0;
    int32_t result = 0;
    int32_t count = sizeof(__safe_storage_extensions)/sizeof(char *);

    // for each file
    while (index < count)
    {
    	// generate filename of each file
        _filename = filename + __safe_storage_extensions[index++];

		C_LOG_DEBUG("deleting file %s", _filename.c_str());
		if (unlink(_filename.c_str()))
        	--result;
    }
    return result;
}

void CSafeStorage::setOldestOffset ( CSafeStorageIndexReg &index, tserial_t serial, int64_t offset, tseq_t sequence, uint32_t flags, bool init )
{
	C_LOG_TRACE("In #:%ld off:%lld seq:%d flgs:%08X init:%d 0:[%d,%lld] 1:[%d,%lld]", serial, offset, sequence, flags, init?1:0, index.sequences[0], index.offsets[0],index.sequences[1], index.offsets[1]);
 	if (init) {
		memset(&index, 0, sizeof(index));
 		index.offsets[0] = offset;
 		index.sequences[0] = sequence;
 		index.flags[0] = flags;
 		index.slots[0] = slot;
 		index.offsets[1] = -1;
 		index.sequences[1] = 0;
 		index.flags[1] = 0;
		index.slots[1] = 0;
 	}
 	else {
 		int32_t pos = (index.sequences[1] > index.sequences[0] ? 0 : 1);
 		index.offsets[pos] = offset;
 		index.sequences[pos] = sequence;
 		index.flags[pos] = flags;
 		index.slots[pos] = slot;
 	}
	C_LOG_TRACE("Out #:%ld 0:[%d,%lld] 1:[%d,%lld]", serial, index.sequences[0], index.offsets[0],index.sequences[1], index.offsets[1]);
}

void CSafeStorage::setNewestOffset ( CSafeStorageIndexReg &index, tserial_t serial, int64_t offset, tseq_t sequence, uint32_t flags )
{
	C_LOG_TRACE("In #:%ld off:%lld seq:%d flgs:%08X 0:[%d,%lld] 1:[%d,%lld]", serial, offset, sequence, flags, index.sequences[0], index.offsets[0],index.sequences[1], index.offsets[1]);
	int32_t pos = (index.sequences[1] > index.sequences[0] ? 1 : 0);
	index.offsets[pos] = offset;
	index.sequences[pos] = offset;
	index.flags[pos] = flags;
	C_LOG_TRACE("Out #:%ld 0:[%d,%lld] 1:[%d,%lld]", serial, index.sequences[0], index.offsets[0],index.sequences[1], index.offsets[1]);
}

int64_t CSafeStorage::getLastOffset ( CSafeStorageIndexReg &index, tserial_t serial, uint32_t flags )
{
	int64_t result = -1;
	tseq_t lastseq = 0;

	CSafeStorageSerialList::iterator it;
	uint32_t dirtyRead = flags & F_CSTORAGE_READ_MODE_MASK;
	for ( int32_t i = 0; i < 2; ++i ) {
		/* check if current position is autocommited if not,
		   check that index is commited, current sequence is lower or equal last transaccion of same slot, if not,
		   check if read in dirty read, F_CSTORAGE_FULL_DIRTY_READ reads dirty of his slot and others, F_CSTORAGE_DIRTY_READ only read dirty of his slot */
		int32_t eval = 0;
		if (((index.flags[i] & F_CSTORAGE_DF_AUTO_COMMIT) || ((eval = 1) == 0) || index.sequences[i] <= state.last_commit_sequences[index.slots[i]] || ((eval = 2) == 0) || 
			((eval = 3) && dirtyRead  && (slot == index.slots[i] || dirtyRead == F_CSTORAGE_FULL_DIRTY_READ) && 
			 (it = dirtySerials.find(serial)) != dirtySerials.end() && (dirtyRead == F_CSTORAGE_FULL_DIRTY_READ || (*it).second == index.slots[i]))) &&
			lastseq <= index.sequences[i]) {
			result = index.offsets[i];
			lastseq = index.sequences[i];
		}
	}
	C_LOG_INFO("0:[%02X, %d,%d,%lld,%d] 1:[%02X, %d,%d,%lld,%d] FLGS:%08X R:%lld", 
		index.flags[0], index.slots[0], index.sequences[0], index.offsets[0], state.last_commit_sequences[index.slots[0]],
		index.flags[1], index.slots[1], index.sequences[1], index.offsets[1], state.last_commit_sequences[index.slots[1]],
		flags, result);
	return result;
}

int64_t CSafeStorage::getLastOffset ( void )
{
	return last_offset;
}

int32_t CSafeStorage::goTop ( uint32_t flags ) 
{
	cursor = -1;
	return E_CSTORAGE_OK;
}

int32_t CSafeStorage::goPos ( tserial_t serial, uint32_t flags  ) 
{
	cursor = serial < 0 ? -1 : serial-1;
	return E_CSTORAGE_OK;
}

/*
int32_t CSafeStorage::readNext ( tserial_t &serial, void *data, uint32_t dlen, uint32_t flags ) 
{
	int32_t result;
	while ((result = read(cursor+1, data, dlen, flags)) == E_CSTORAGE_SERIAL_NOT_FOUND);
	serial = cursor;
	return result;
}
*/

int32_t CSafeStorage::setSlot ( tslot_t slot )
{
	this->slot = slot;
	return E_CSTORAGE_OK;
}

tslot_t CSafeStorage::getSlot ( void )
{
	return slot;
}

int32_t CSafeStorage::getParam ( const string &name )
{
	if (name == "index_cache_size") return findex.getCacheSize();
	else if (name == "log_cache_size") return flog.getCacheSize();
	return -1;
}

int32_t CSafeStorage::setParam ( const string &name, int32_t value )
{
	if (name == "index_cache_size")	findex.setCacheSize(value);
	else if (name == "log_cache_size") flog.setCacheSize(value);
	else if (name == "c_log_level") c_log_set_level(value);
	else return -1;
	return 0;
}

void CSafeStorage::findLastSignatureReg ( int32_t max_size )
{
	fdata.findLastSignatureReg(max_size, CSTORAGE_SIGNATURE, 0xFFFFFFFF ^ CSTORAGE_SIGNATURE_MASK, sizeof(uint32_t));
}
