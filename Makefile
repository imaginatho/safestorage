all: lib/libsafestorage.so bin/safe_test bin/read_example bin/write_example

lib/libsafestorage.so:
	@echo -e "\x1B[34m############## compiling libsafestorage ##############\x1B[0m"
	(cd src/lib && make)

bin/safe_test:
	@echo -e "\x1B[34m############## compiling safe_test ##############\x1B[0m"
	(cd src/test && make)

bin/read_example:
	@echo -e "\x1B[34m############## compiling read_example ##############\x1B[0m"
	(cd src/examples/read && make)

bin/write_example:
	@echo -e "\x1B[34m############## compiling write_example ##############\x1B[0m"
	(cd src/examples/write && make)
	
clean:
	(cd src/lib && make clean)
	(cd src/test && make clean)
	(cd src/examples/read && make clean)
	(cd src/examples/write && make clean)
