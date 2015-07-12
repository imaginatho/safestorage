#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

using namespace std;

#include <exception.h>
#include <safe_storage_connection.h>
#include <safe_storage_listener.h>

#define BLOCK_SIZE 	(4*1024)

CSafeStorageConnection::CSafeStorageConnection ( int fd, struct sockaddr &in_addr )
	:fd(fd)
{
	dsize = 128 * 1024;
	dbegin = (uint8_t *)malloc(dsize);
	dend = dbegin + dsize;
	dcur = dbegin;
}

CSafeStorageConnection::~CSafeStorageConnection ( void )
{
	if (fd >= 0) close(fd);
}

void CSafeStorageConnection::realloc ( uint32_t size )
{
	if (dsize >= size) return;
	dsize = ((size / BLOCK_SIZE) + (size % BLOCK_SIZE ? 1: 0)) * BLOCK_SIZE;
	printf("*** realloc(%d => %d)\n", size, dsize);

	uint32_t doffset = dcur ? dcur - dbegin : 0;
	dbegin = (uint8_t *)::realloc(dbegin, dsize);
	dend = dbegin + dsize;
	if (dcur) dcur = dbegin + doffset;
}

int32_t CSafeStorageConnection::onData ( void )
{
	int bytes, dread, dmax;

	while (1) {
		dread = dcur - dbegin;
		
		if (dread < sizeof(CSAFE_NET_HEADER)) dmax = sizeof(CSAFE_NET_HEADER) - dread;
		else dmax = ((CSAFE_NET_HEADER *)dbegin)->len - dread;

		printf("dread:%-8d dmax:%-8d sizeof:%-8d\n", dread, dmax, sizeof(CSAFE_NET_HEADER));
		if ((dcur + dmax) > dend) realloc(dread + dmax);

		if (dmax == 0) {
			onRequest((CSAFE_NET_HEADER *)dbegin, dbegin + sizeof(CSAFE_NET_HEADER), dcur - dbegin - sizeof(CSAFE_NET_HEADER));
			dcur = dbegin;
			continue;
		}

		bytes = read (fd, dcur, dmax);
		printf("bytes:%-8d dmax:%-8d\n", bytes, dmax);
		if (bytes > 0) {
			dcur += bytes;
			continue;
		}

		if (bytes < 0 && errno == EAGAIN) return 0;
		break;
	}
	return -1;
}

int32_t CSafeStorageConnection::onRequest ( CSAFE_NET_HEADER *hdr, uint8_t *data, uint32_t dlen )
{
	printf("hdr.signature:    %08X\n", hdr->signature);
	printf("hdr.len:          %d\n", hdr->len);
	printf("hdr.cmd_res:      %d\n", hdr->cmd_res);
	printf("dlen:             %d\n", dlen);
	if (dlen > 0) printf("=== BEGIN MSG ===\n%s\n=== END MSG ===\n\n", data);
	
	switch (hdr->cmd_res) {
		case SAFE_CMD_OPEN:
			break;
			
		case SAFE_CMD_CLOSE:
			break;
			
		case SAFE_CMD_READ:
			break;
			
		case SAFE_CMD_WRITE:
			break;
			
		case SAFE_CMD_VERIFY:
			break;
			
		case SAFE_CMD_READ_LOG:
			break;
			
		case SAFE_CMD_APPLY_LOG:
			break;
			
		case SAFE_CMD_READ_LOG_REG:
			break;
			
		case SAFE_CMD_COMMIT:
			break;
			
		case SAFE_CMD_ROLLBACK:
			break;
			
		case SAFE_CMD_GET_PARAM:
			break;
			
		case SAFE_CMD_SET_PARAM:
			break;
			
		case SAFE_CMD_GET_INFO:
			break;
			
	}
	return 0;
}

#undef BLOCK_SIZE