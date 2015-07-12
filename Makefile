all: libsafestorage test examples benchmarks test

libsafestorage:
	@echo -e "\x1B[34m############## compiling libsafestorage ##############\x1B[0m"
	@(cd src/lib && make)

test:
	@echo -e "\x1B[34m############## compiling test ##############\x1B[0m"
	@(cd src/test && make)

examples:
	@echo -e "\x1B[34m############## compiling examples ##############\x1B[0m"
	@(cd src/examples && make)

benchmarks:
	@echo -e "\x1B[34m############## compiling benchmarks ##############\x1B[0m"
	@(cd src/benchmarks && make)
	
clean:
	@echo -e "\x1B[34m############## cleaning libsafestorage ##############\x1B[0m"
	@(cd src/lib && make clean)
	@echo -e "\x1B[34m############## cleaning test ##############\x1B[0m"
	@(cd src/test && make clean)
	@echo -e "\x1B[34m############## cleaning examples ##############\x1B[0m"
	@(cd src/examples && make clean)
	@echo -e "\x1B[34m############## cleaning benchmarks ##############\x1B[0m"
	@(cd src/benchmarks && make clean)