#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <libgen.h>

#include <safe_storage.h>

#define N_RECORDS 10

int main ( int argc, char *argv[] )
{
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
}