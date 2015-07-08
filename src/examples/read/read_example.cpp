#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>

#include <safe_storage.h>

int main ( int argc, char *argv[] )
{
	if (argc != 2) {
		printf("usage: %s <safe_storage_filename>\n", basename(argv[0]));
		exit(-1);
	}

	ISafeStorage *sfs = createISafeStorage();
	if (sfs->open(argv[1]) != E_CSTORAGE_OK) {
		printf("ERROR opening SFS file %s\n", argv[1]);
		exit(-2);
	}

	char data[1024];
	int bytes = 0;
	tserial_t key = 1;
	
	while (bytes >= 0) {
		bytes = sfs->read(key, data, sizeof(data));
		if (bytes >= 0) {
			printf("#%u[%d]:%s\n", key, bytes, data);
		}
		++key;
	}
	sfs->close();
}