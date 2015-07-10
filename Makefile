all: libsafestorage safe_test read_example write_example ini_example

libsafestorage:
	@echo -e "\x1B[34m############## compiling libsafestorage ##############\x1B[0m"
	@(cd src/lib && make)

safe_test:
	@echo -e "\x1B[34m############## compiling safe_test ##############\x1B[0m"
	@(cd src/test && make)

read_example:
	@echo -e "\x1B[34m############## compiling read_example ##############\x1B[0m"
	@(cd src/examples/read && make)

write_example:
	@echo -e "\x1B[34m############## compiling write_example ##############\x1B[0m"
	@(cd src/examples/write && make)

ini_example:
	@echo -e "\x1B[34m############## compiling ini_example ##############\x1B[0m"
	@(cd src/examples/ini && make)
	
clean:
	@echo -e "\x1B[34m############## cleaning libsafestorage ##############\x1B[0m"
	@(cd src/lib && make clean)
	@echo -e "\x1B[34m############## cleaning safe_test ##############\x1B[0m"
	@(cd src/test && make clean)
	@echo -e "\x1B[34m############## cleaning read_example ##############\x1B[0m"
	@(cd src/examples/read && make clean)
	@echo -e "\x1B[34m############## cleaning write_example ##############\x1B[0m"
	@(cd src/examples/write && make clean)
	@echo -e "\x1B[34m############## cleaning ini_example ##############\x1B[0m"
	@(cd src/examples/ini && make clean)
