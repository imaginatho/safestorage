all: read_example write_example ini_example listener_example replica_example

read_example:
	@echo -e "\x1B[34m############## compiling read_example ##############\x1B[0m"
	@(cd read && make)

write_example:
	@echo -e "\x1B[34m############## compiling write_example ##############\x1B[0m"
	@(cd write && make)

listener_example:
	@echo -e "\x1B[34m############## compiling listener_example ##############\x1B[0m"
	@(cd listener && make)

replica_example:
	@echo -e "\x1B[34m############## compiling replica_example ##############\x1B[0m"
	@(cd replica && make)

ini_example:
	@echo -e "\x1B[34m############## compiling ini_example ##############\x1B[0m"
	@(cd ini && make)
	
clean:
	@echo -e "\x1B[34m############## cleaning read_example ##############\x1B[0m"
	@(cd read && make clean)
	@echo -e "\x1B[34m############## cleaning write_example ##############\x1B[0m"
	@(cd write && make clean)
	@echo -e "\x1B[34m############## cleaning listener_example ##############\x1B[0m"
	@(cd listener && make clean)
	@echo -e "\x1B[34m############## cleaning replica_example ##############\x1B[0m"
	@(cd replica && make clean)
	@echo -e "\x1B[34m############## cleaning ini_example ##############\x1B[0m"
	@(cd ini && make clean)
