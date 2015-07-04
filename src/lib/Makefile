# *****************************************************
# Variables to control Makefile operation

include ../Makefile.defaults

# ****************************************************
# Targets needed to bring the executable up to date

MODULES=c_log.o data_array.o safe_file_exception.o safe_storage.o shared_safe_storage.o base_safe_file.o open_safe_storage.o\
 safe_client.o factory.o crc32.o
 
TARGET=../../lib/libsafestorage.so

all: $(TARGET)

$(TARGET): $(MODULES)
	$(CXX) $(CXXFLAGS) -shared $(MODULES) -o $@

crc32.o: crc32.cpp
	$(CXX) $(CXXFLAGS) -O3 -c $*.cpp -o $@
		
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $*.cpp -o $@

clean:
	rm *.o $(TARGET)
	

