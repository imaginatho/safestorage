#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <c_log.h>
#include <safe_storage.h>
#include <math.h>

#define RND_PHASE_OFFSET	(RAND_MAX/100)
#define MAX_REG_SIZE	(250*1024)
#define MIN_REG_SIZE	1
static char random_data[MAX_REG_SIZE];

#define CHECK_OP(OP,R,TEXT) {int32_t _tmp=OP;if (_tmp!=R){printf(TEXT,_tmp);exit(-1);}}

#define SZ_GB	(1024LL*SZ_MB)
#define SZ_MB	(1024LL*SZ_KB)
#define	SZ_KB	1024LL
#define DO_TEST_TITLE(X,Y) printf(X);if (Y){exit(-1);}

const char *size_str ( int64_t value, char *dst = NULL )
{
	static char _tmp[200];
	const char *unit;
	int64_t factor;
	if (!dst) {
		dst = _tmp;
	}
	
		if (value >= SZ_GB) {
		factor = SZ_GB;
		unit = "GB";
	} 
	else if (value >= SZ_MB) {	
		factor = SZ_MB;
		unit = "MB";
	} 
	else if (value >= SZ_KB) {
		factor = SZ_KB;
		unit = "KB";
	} 
	else {
		sprintf(dst, "%lld bytes", value);
		return dst;
	}
	
	double uv = (double)value/factor;
	double ipart, fpart;
	
	fpart = modf(uv, &ipart);
	if (fpart >= 0.1) { 
		sprintf(dst, "%.2f %s", uv, unit);
	}
	else {
		sprintf(dst, "%lld %s", (int64_t)ipart, unit);
	}
	return dst;	
}

const char *result_str ( int32_t value )
{
	static char _tmp[200];
	switch (value)
	{
		case E_CSTORAGE_OK: 						return "E_CSTORAGE_OK";
		case E_CSTORAGE_ERROR:						return "E_CSTORAGE_ERROR";
		case E_CSTORAGE_OPEN:						return "E_CSTORAGE_OPEN";
		case E_CSTORAGE_CREATE:						return "E_CSTORAGE_CREATE";
		case E_CSTORAGE_CREATE_FILE_EXISTS:			return "E_CSTORAGE_CREATE_FILE_EXISTS";
		case E_CSTORAGE_OPEN_FILE_NO_ACCESS:		return "E_CSTORAGE_OPEN_FILE_NO_ACCESS";
		case E_CSTORAGE_DATA_READ:					return "E_CSTORAGE_DATA_READ";
		case E_CSTORAGE_SERIAL_NOT_FOUND:			return "E_CSTORAGE_SERIAL_NOT_FOUND";
		case E_CSTORAGE_NOT_OPENED:					return "E_CSTORAGE_NOT_OPENED";
		case E_CSTORAGE_ALREADY_OPENED:				return "E_CSTORAGE_ALREADY_OPENED";
		case E_CSTORAGE_NO_ROLLBACK_IN_AUTOCOMMIT:	return "E_CSTORAGE_NO_ROLLBACK_IN_AUTOCOMMIT";
		case E_CSTORAGE_SEQUENCE_NOT_FOUND:			return "E_CSTORAGE_SEQUENCE_NOT_FOUND";
		case E_CSTORAGE_NOT_ALLOWED_IN_READ_MODE:	return "E_CSTORAGE_NOT_ALLOWED_IN_READ_MODE";
		case E_CSTORAGE_ONLY_ONE_WRITER:			return "E_CSTORAGE_ONLY_ONE_WRITER";
		case E_CSTORAGE_UNKNOWN_COMMAND:			return "E_CSTORAGE_UNKNOWN_COMMAND";
		case E_CSTORAGE_ONLT_ONE_WRITTER:			return "E_CSTORAGE_ONLT_ONE_WRITTER";
		case E_CSTORAGE_LOCKED_REGISTER:			return "E_CSTORAGE_LOCKED_REGISTER";
	}
	snprintf(_tmp, sizeof(_tmp), "%d", value);
	return _tmp;
}

int32_t random_reg ( int32_t index, char *data = random_data, int32_t max_reg_size = MAX_REG_SIZE, int32_t min_reg_size = MIN_REG_SIZE, int32_t phase = 0 )
{
	if (max_reg_size > MAX_REG_SIZE) {
		max_reg_size = MAX_REG_SIZE;
	}
	srand((index* RND_PHASE_OFFSET * (phase+1))+1);
	int32_t bytes = rand() % (max_reg_size+1);
	if ( bytes < min_reg_size ) {
		bytes = min_reg_size;
	}
	int32_t count = bytes;
	int32_t *d32 = (int32_t *)data;
	while (count >= 4) {
		*d32 = rand();
		++d32;
		count-=4;
	}
	data = (char *)d32;
	while (count > 0) {
		*data = (char)rand();
		++data;
		--count;
	}
	return bytes;
}

int32_t check_reg ( int32_t index, char *data, int32_t dlen, int32_t max_reg_size = MAX_REG_SIZE, int32_t min_reg_size = MIN_REG_SIZE, int32_t phase = 0 )
{
	if (max_reg_size > MAX_REG_SIZE) {
		max_reg_size = MAX_REG_SIZE;
	}
	srand((index* RND_PHASE_OFFSET * (phase+1))+1);
	int32_t bytes = rand() % (max_reg_size+1);
	if ( bytes < min_reg_size ) {
		bytes = min_reg_size;
	}
	if (dlen != bytes) return -1;
	int32_t count = bytes;
	int32_t *d32 = (int32_t *)data;
	while (count >= 4) {
		if (*d32 != rand()) return -2;
		++d32;
		count-=4;
	}
	data = (char *)d32;
	while (count > 0) {
		if (*data != (char)rand()) return -3;
		++data;
		--count;
	}
	return 0;
}

// test to verify random, must be pseudo-random
int32_t random_test ( void )
{
	int32_t result, bytes;
	for(int32_t index = 0; index < 100; ++index) {
		bytes = random_reg(index, random_data);
		result = check_reg(index, random_data, bytes);
		if (result)
			return result;
	}
	return 0;
}

int32_t write_registers ( ISafeStorage *ssi, int32_t from, int32_t to, int32_t commit_each = 0 , 
						  int32_t max_reg_size = MAX_REG_SIZE, int32_t phase = 0, int32_t ecode = 0 )
{
	int32_t bytes, index = 0;
	int32_t result;
	
	while (from <= to) {
		bytes = random_reg(from, random_data, max_reg_size, MIN_REG_SIZE, phase);
		result = ssi->write(from, random_data, bytes);
		printf("## [%08d] write (%s)", from, result_str(result));
		if ((result < 0 && !ecode) || (result >=0 && ecode <0)) {
			printf("FAIL\n");
			return -1;
		}
		if (ecode < 0 && result != ecode) {
			printf("FAIL\n");
			return -3;
		}
		if  (commit_each && ((index + 1) % commit_each) == 0) {
			printf("(C)");
			ssi->commit();
		}
		printf(" - OK\n");
		++index;
		++from;
	}
	return 0;
} 

int32_t verify_registers ( ISafeStorage *ssi, tserial_t from, int32_t to, int32_t max_reg_size = MAX_REG_SIZE, uint32_t flags = 0, int32_t ecode = 0, int32_t phase = 0 )
{
	int32_t step = 1;
	if (to < 0) {
		step = 3;
		to = -to;
	}
	while (from <= to) 
	{
//		printf("## [%08d] read\n", from);
		int32_t bytes = ssi->read(from, random_data, sizeof(random_data), flags);
		printf("## [%08d] read data(%s) - ", from, result_str(bytes));
		if ((bytes < 0 && !ecode) || (bytes >=0 && ecode <0)) {
			printf("FAIL\n");
			return -1;
		}
		if (ecode >=0 && check_reg(from, random_data, bytes, max_reg_size, MIN_REG_SIZE, phase)) {
			printf("FAIL\n");
			return -2;
		}
		if (ecode < 0 && bytes != ecode) {
			printf("FAIL\n");
			return -3;
		}
		printf("OK\n");
		from += step;
	}
	return 0;
}

#define F_MODE_NORMAL	0
#define F_MODE_READ		1
#define F_MODE_WRITE	2
#define F_MODE_LOG      3

int32_t main ( int32_t argc, char *argv[] )
{
	int32_t bytes, opt;
	int32_t dlevel = 0;
	int32_t nloops = 10000;
	int32_t mode = 0;
	int32_t times = 10;
	int32_t msleep = 0;
	uint32_t commit_freq = 1000;
	uint32_t initial_key = 1;
	bool flg_create = false;
	bool flg_empty = false;
	int32_t max_reg_size = MAX_REG_SIZE;
	const char *filename = "0001";
	char *filename2 = NULL;
		
    while ((opt = getopt(argc, argv, "d:f:rlwn:m:pt:s:cI:C:E:A:")) != -1) {
        switch (opt) {
			case 'C':
				commit_freq = atol(optarg);
				break;
			case 'd':
				dlevel = atoi(optarg);
				break;
			case 'c':
				flg_create = true;
				break;
			case 'E':
				flg_empty = true;
				break;
			case 'I':
				initial_key = atol(optarg);
				break;
			case 's':
				msleep = atoi(optarg);
				break;
			case 'f':
				filename = strdup(optarg);
				break;
			case 'A':
				filename2 = strdup(optarg);
				break;
			case 'n':
				nloops = atoll(optarg);
				break;
			case 't':
				times = atoi(optarg);
				break;
			case 'm':
				max_reg_size = atoll(optarg);
				break;
			case 'r':
				mode = F_MODE_READ;
				break;
			case 'l':
				mode = F_MODE_LOG;
				break;
			case 'w':
				mode = F_MODE_WRITE;
				break;
			default: /* '?' */
				fprintf(stderr, "Usage: %s [-d debuglevel] [-d <max_reg_size>] [-r|-w] [-t <times>] [-n <loops>] [-p] [-f <filename>]\n", argv[0]);
				exit(EXIT_FAILURE);
		}
    }

	ISafeStorage *ssi = createISafeStorage();
	ISafeStorage *_ssi = createISafeStorage();
	
	if (dlevel > 0) {
		ssi->setParam("c_log_level", dlevel);
		_ssi->setParam("c_log_level", dlevel);
	}
	
	if (mode == F_MODE_WRITE) {
		printf("Write Mode\n");
		ssi->open(filename, F_CSTORAGE_WR | (flg_create ? F_CSTORAGE_CREATE : 0));

		for (int32_t index = 0; index < nloops; ++index) {
			bytes = random_reg(index + initial_key, random_data, max_reg_size);
			printf("WRITE |%10d|%6d\n", index + initial_key, bytes);
			if (ssi->write(index + initial_key, random_data, bytes) < 0) {
				printf("found error on index %d\n", index + initial_key);
				exit(-1);
			}
			if (index && index % commit_freq == 0) {
				printf("COMMIT|%10d|0\n", index + initial_key);
				ssi->commit();
			}
			if (msleep) usleep(msleep * 1000);
		}
		printf("COMMIT|%10d|0\n", 0);
		ssi->commit();
		exit(0);
	}

	if (mode == F_MODE_READ) {
		ssi->open(filename);

		int32_t bytes, count = 0;
		int64_t tot_bytes = 0;

		tserial_t index = 0;
		ssi->goTop();
		while (true) {
			if (nloops <= 0) break;
			--nloops;
			memset(random_data, 0, sizeof(random_data));
			
			tserial_t key = index;
			bytes = ssi->read(key , random_data, sizeof(random_data) ,true);
			if (bytes == E_CSTORAGE_SERIAL_NOT_FOUND) {
				if (flg_empty) printf("*EMPTY|%8d|%8d\n", index, key);
				++index;
				++nloops;
				continue;
			}
			if (bytes < 0) {
				printf("found error on index %d (%s) COUNT:%d TOT_BYTES:%lld\n", index, result_str(bytes), count, tot_bytes);
				if (bytes == E_CSTORAGE_SERIAL_NOT_FOUND) continue;
				exit(-1);
			}
			printf("READ  |%10d|%6d\n", index, bytes);
			tot_bytes += bytes;
			if (check_reg(index, random_data, bytes, max_reg_size)) {
				printf("faild check index %d COUNT:%d\n", index, count);
				exit(-2);
			}		
			++index;
			++count;
			if (msleep) usleep(msleep * 1000);
		}
		exit(0);
	}

	if (mode == F_MODE_LOG) {
		ssi->open(filename);
		if (filename2) {
			CSafeStorageInfo info;
			ssi->getInfo(info);
			printf("COUNT:%d\n", info.count);
			_ssi->open(filename2, F_CSTORAGE_WR|F_CSTORAGE_CREATE|F_CSTORAGE_SET_HASH_KEY, info.hash_key);
		}
		
		int32_t bytes, count = 0;
		int64_t tot_bytes = 0;
		tseq_t index = 1;
		ssi->goTop();
		while (true) {
			if (nloops <= 0) break;
			--nloops;
			memset(random_data, 0, sizeof(random_data));
			bytes = ssi->readLog(index, random_data, sizeof(random_data));
			if (bytes < 0) {
				printf("found error on index %d (%s) COUNT:%d TOT_BYTES:%lld\n", index, result_str(bytes), count, tot_bytes);
				if (bytes == E_CSTORAGE_SERIAL_NOT_FOUND) continue;
				exit(-1);
			}
			CSafeStorageLogInfo linfo;
			ssi->getLogReg(random_data, bytes, linfo);
			const char *label = "??????";
			switch (linfo.type) {
				case T_CSTORAGE_COMMIT:         label = "COMMIT"; break;
				case T_CSTORAGE_WRITE:          label = "WRITE"; break;
				case T_CSTORAGE_ROLLBACK_BEGIN: label = "ROLB_B"; break;
				case T_CSTORAGE_ROLLBACK_END:   label = "ROLB_E"; break;
				case T_CSTORAGE_STATUS:         label = "STATUS"; break;
			}
			
			printf("%-6s|%10d|%10d|%06d|%08X|%6d\n",label, linfo.sequence, linfo.serial, linfo.len, linfo.flags, bytes);
			tot_bytes += bytes;
			if (linfo.serial != 0 && check_reg(linfo.serial, random_data, bytes, max_reg_size)) {
				printf("faild check index %d COUNT:%d\n", index, count);
				exit(-2);
			}
			if (filename2) {
				bytes = _ssi->applyLog(random_data, bytes);
				if (bytes < 0) {
					printf("found error on appling index %d (%s) serial: %d\n", index, result_str(bytes), linfo.serial);
					exit(-1);
				}
			}
			++count;
			++index;
		}
		if (filename2) {
			_ssi->commit();
			_ssi->close();
		}
		exit(0);
	}
	
	printf ("## 1. Random Compatibility Test\n");
	if (random_test()) {
		printf("## no compatible random test found\n");
		exit(-1);
	}

	printf ("## 2 Creation Test\n");
	ssi->open(filename, F_CSTORAGE_WR|F_CSTORAGE_CREATE);
	
	int32_t index = 1;

	printf ("## 3 Write Test\n");
	int32_t reg_sizes [] = { 10, 100, 1024, 10*1024, 50*1024, 100*1024, 200*1024 };
	index = 1;
	for (int32_t isize = 0; isize < (int32_t)(sizeof(reg_sizes)/sizeof(reg_sizes[0])); ++isize) {
		printf ("## 3.%d Write Registers up to %s Test\n", isize+1, size_str(reg_sizes[isize]));
		write_registers(ssi, index, index+(times-1), 0, reg_sizes[isize]);
		ssi->commit();
		index += times;
	}
	
	index = 1;
	for (int32_t isize = 0; isize < (int32_t)(sizeof(reg_sizes)/sizeof(reg_sizes[0])); ++isize) {
		printf ("## 3.%d Read Registers up to %s Test\n", isize+1+(sizeof(reg_sizes)/sizeof(reg_sizes[0])), size_str(reg_sizes[isize]));
		if (verify_registers(ssi, index, index+(times-1), reg_sizes[isize])) {
			exit(-1);
		}
		index += times;
	}

	write_registers(ssi, index, index, 0, 0);
	ssi->commit();
	DO_TEST_TITLE("## 4 Empty register test\n", verify_registers(ssi, index, index, 0));
	++index;
	
	DO_TEST_TITLE("## 5 Transactional Test\n", write_registers(ssi, index, index+(times-1), 0));
	_ssi->open(filename);
	
	DO_TEST_TITLE ("## 5.0 PreTest dirty read read\n", verify_registers(ssi, index, index+(times-1)));
	DO_TEST_TITLE ("## 5.1 Test commited read\n", verify_registers(_ssi, index, index+(times-1), MAX_REG_SIZE, F_CSTORAGE_COMMITED_READ, E_CSTORAGE_SERIAL_NOT_FOUND));
	DO_TEST_TITLE ("## 5.2 Test dirty read read\n", verify_registers(_ssi, index, index+(times-1), MAX_REG_SIZE, F_CSTORAGE_DIRTY_READ));

	printf ("## 6 Rollback Test\n");
	CHECK_OP(ssi->rollback(), 0, "Rollback fails with code:%d\n")
	DO_TEST_TITLE("## 6.1 Test commited read\n", verify_registers(ssi, index, index+(times-1), MAX_REG_SIZE, F_CSTORAGE_COMMITED_READ, E_CSTORAGE_SERIAL_NOT_FOUND));
	DO_TEST_TITLE("## 6.2 Test dirty read test\n", verify_registers(ssi, index, index+(times-1), MAX_REG_SIZE, F_CSTORAGE_DIRTY_READ, E_CSTORAGE_SERIAL_NOT_FOUND));
	index += times;

	DO_TEST_TITLE("## 7 Update test\n", write_registers(ssi, index, index+(times-1), 0));
	CHECK_OP(ssi->commit(), 0, "Commit fails with code:%d\n")	

	DO_TEST_TITLE("## 7.1 Read test (once written)\n", verify_registers(ssi, index, index+(times-1), MAX_REG_SIZE, F_CSTORAGE_COMMITED_READ));
	write_registers(ssi, index, index+(times-1), 0, MAX_REG_SIZE, 1);	
	DO_TEST_TITLE("## 7.2 Read test (twice written)\n", verify_registers(_ssi, index, index+(times-1)));
	DO_TEST_TITLE("## 7.3 Dirty Read Test (twice written)\n", verify_registers(ssi, index, index+(times-1), MAX_REG_SIZE, F_CSTORAGE_DIRTY_READ, 0, 1));
	CHECK_OP(ssi->commit(), 0, "Commit fails with code:%d\n")
	DO_TEST_TITLE("## 7.4 Commited Read after commit (twice written)\n", verify_registers(ssi, index, index+(times-1), MAX_REG_SIZE, F_CSTORAGE_COMMITED_READ, 0, 1));
	DO_TEST_TITLE("## 7.5 Dirty Read Test after commit (twice written)\n", verify_registers(ssi, index, index+(times-1), MAX_REG_SIZE, F_CSTORAGE_DIRTY_READ, 0, 1));
	/*
	DO_TEST_TITLE("## 8 RollBack Test\n", write_registers(ssi, index, index+(times-1), 0, MAX_REG_SIZE, 2));
	DO_TEST_TITLE("## 8.1 Read test after write before rollback\n", verify_registers(ssi, index, index+(times-1), MAX_REG_SIZE, F_CSTORAGE_COMMITED_READ, 0, 1));
	DO_TEST_TITLE("## 8.2 Dirty Read test after write before rollback\n", verify_registers(ssi, index, index+(times-1), MAX_REG_SIZE, F_CSTORAGE_DIRTY_READ, 0, 2));
	CHECK_OP(ssi->rollback(), 0, "Rollback fails with code:%d\n")
	DO_TEST_TITLE("## 8.3 Read Test\n", verify_registers(ssi, index, index+(times-1), MAX_REG_SIZE, F_CSTORAGE_COMMITED_READ, 0, 1));
	DO_TEST_TITLE("## 8.4 Dirty Read Test\n", verify_registers(ssi, index, index+(times-1), MAX_REG_SIZE, F_CSTORAGE_DIRTY_READ, 0, 1));
	index += times;
	DO_TEST_TITLE("## 9 Rollback after commit test\n", write_registers(ssi, index, index+(times-1), 0));	
	DO_TEST_TITLE("## 9.1 Read test previus registers\n", verify_registers(ssi, index-times, index-1, MAX_REG_SIZE, F_CSTORAGE_COMMITED_READ, 0, 1));
	DO_TEST_TITLE("## 9.2 Dirty Read test previus registers\n", verify_registers(ssi, index-times, index-1, MAX_REG_SIZE, F_CSTORAGE_DIRTY_READ, 0, 1));
	CHECK_OP(ssi->commit(), 0, "Commit fails with code:%d\n")	
	DO_TEST_TITLE("## 9.3 Read test previus registers after commit\n", verify_registers(ssi, index-times, index-1, MAX_REG_SIZE, F_CSTORAGE_COMMITED_READ, 0, 1));
	DO_TEST_TITLE("## 9.4 Dirty Read test previus registers after commit\n", verify_registers(ssi, index-times, index-1, MAX_REG_SIZE, F_CSTORAGE_DIRTY_READ, 0, 1));
	DO_TEST_TITLE("## 9.5 Read test after commit\n", verify_registers(ssi, index, index+(times-1), MAX_REG_SIZE, F_CSTORAGE_COMMITED_READ));
	*/
	index += times;
	
	int32_t deltas[] = { 0, 2, 10, 15, 20, 35, 50};
	int32_t n_deltas = sizeof(deltas)/sizeof(deltas[0]);
	int32_t idelta;
	printf ("## 10 Non consecutive writes\n");
	for (idelta = 0; idelta < n_deltas; ++idelta) {
		write_registers(ssi, index+deltas[idelta], index+deltas[idelta], 0);
	}
	CHECK_OP(ssi->commit(), 0, "Commit fails with code:%d\n")	
	idelta = 0;
	printf ("## 10.1 Read registers Tests\n");
	for (int32_t delta = 0; delta <= deltas[n_deltas-1]; ++delta) {
		verify_registers(ssi, index+delta, index+delta, MAX_REG_SIZE, F_CSTORAGE_COMMITED_READ, deltas[idelta] == delta ? 0 : E_CSTORAGE_SERIAL_NOT_FOUND);
		if (deltas[idelta] == delta && idelta < (n_deltas-1)) {
			++idelta;
		}
	}
	
	ssi->close();
	return 0;
}
