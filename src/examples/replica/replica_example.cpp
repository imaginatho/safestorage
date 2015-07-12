#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <libgen.h>
#include <stdint.h>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

extern int h_errno;

#include <safe_storage.h>
#include <safe_storage_net.h>

#define N_RECORDS 10

int main ( int argc, char *argv[] )
{
	int sfd;
	struct sockaddr_in saddr;
	struct hostent *hostp;
	static uint8_t _data[200 * 1024];

	if ((sfd = socket(AF_INET , SOCK_STREAM , 0)) < 0) {
		perror("Client-connect() error");
		exit(-1);
	}

	memset(&saddr, 0, sizeof(saddr));

	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(9080);

	hostp = gethostbyname("127.0.0.1");
	if (!hostp) {
		printf("ERROR %d\n", h_errno);
		close(sfd);
		exit(-1);
	}

	memcpy(&saddr.sin_addr, hostp->h_addr, sizeof(saddr.sin_addr));
	if(connect(sfd, (struct sockaddr *)&saddr, sizeof(saddr)) < 0) {
		perror("Client-connect() error");
		close(sfd);
		exit(-1);
	}

	CSAFE_NET_HEADER *hdr = (CSAFE_NET_HEADER *)_data;
	uint8_t *data = _data + sizeof(CSAFE_NET_HEADER);
	const char msg[] = "hello server!!";
	
	hdr->signature = SAFE_NET_HEADER_SIGNATURE;
	hdr->len = sizeof(CSAFE_NET_HEADER) + sizeof(msg);
	hdr->cmd_res = 1023;
	
	printf("sizeof:%d msg:%d\n", sizeof(CSAFE_NET_HEADER), sizeof(msg));
	
	printf("TEST 1\n");
	strcpy((char *)data, (const char *)msg);
	int bytes = write(sfd, _data, hdr->len);
	printf("%d/%d bytes written\n", bytes, hdr->len);
	sleep(2);
	printf("TEST 2\n");
	for (int index = 0; index < hdr->len; ++index) {
		write(sfd, _data + index, 1);
		usleep(250000);
	}
	sleep(2);
	printf("TEST 3\n");
	CSAFE_NET_HEADER *hdr2 = (CSAFE_NET_HEADER *)(_data + hdr->len);
	
	memcpy(hdr2, _data, hdr->len);
	hdr2->cmd_res = 1024;
	bytes = write(sfd, _data, hdr->len * 2);
	printf("%d/%d bytes written\n", bytes, hdr->len);
	sleep(2);

	printf("TEST 4\n");
	int dsize = (130 * 1024) + 10;
	hdr->len = sizeof(CSAFE_NET_HEADER) + dsize;
	memset(data, 'A', dsize - 3);
	data[dsize-3] = 'B';
	data[dsize-2] = 'C';
	data[dsize-1] = '\0';
	
	bytes = write(sfd, _data, hdr->len);
	printf("%d/%d bytes written\n", bytes, hdr->len);
	sleep(10);
	close(sfd);
/*
	if (argc != 2) {
		printf("usage: %s <safe_storage_filename>\n", basename(argv[0]));
		exit(-1);
	}

	ISafeStorage *sfs = createISafeStorage();
	if (sfs->open(argv[1], F_CSTORAGE_CREATE|F_CSTORAGE_WR) != E_CSTORAGE_OK) {
		printf("ERROR opening/creating SFS file %s\n", argv[1]);
		exit(-2);
	}

	int bytes = 0;
	tserial_t key = 1;

	const char *records[N_RECORDS] = {"One 1", "Two 2", "Three 3", "Four 4", "Five 5", "Six 6", "Seven 7", "Eight 8", "Nine 9", "Ten 10"};

	while (key <= N_RECORDS) {
		bytes = sfs->write(key, records[key-1], strlen(records[key-1])+1);
		if (bytes > 0) {
			printf("#%u[%d]:%s\n", key, bytes, records[key-1]);
		}
		++key;
	}
	sfs->commit();
	sfs->createListener("9080");
	sleep(60);
	sfs->close();
*/
}