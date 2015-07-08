## Synopsis

This project was a implentation of master transactional log file, in other words, a library to store in safe way information about transaccions. This information was stored in secure way, to do it use a crc32 code for each register stored. This project could be used if you need to store sequencial transaccions (not need to insert sequencial, but holes in transaccion id have a cost). Implements two indexes on for transaccional id, but not implements a hash o b-tree, implements a table, for this reason holes have a cost. The second index was for logs, used normaly if need to replicate this file in other host.

## Code Example

This is an example to write 10 records
```C++
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
	sfs->close();
```

This is an example to read records
```C++
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
```

## Motivation

This project born in 2013, used internally and not availaible in github. At this year proyect was available, and we continue to make updates. This project born to resolve a specific problem, in way to find the best performance solution. We saw this kind of file in a lot of transaccionals systems, perhaps not new systems, but very good performance systems.

## Installation

To use this project only need library libsafestorage.so and safe_storage.h header file. To compile project only need execute make on root of project.

## API Reference

Working on this section

## License

This project have a very flexible license, a MIT license, read LICENCE file to more information
