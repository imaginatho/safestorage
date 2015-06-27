all: libsafestorage

libsafestorage:
	(cd src/lib && make)
	
clean:
	(cd src/lib && make clean)
