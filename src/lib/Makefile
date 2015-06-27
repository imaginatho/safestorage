# *****************************************************
# Variables to control Makefile operation

CXX=g++
CXXFLAGS=-Wall -g -m32 -D_DEBUG_LEVEL_=3 -Iinclude -I../include -I../../include

# ****************************************************
# Targets needed to bring the executable up to date

# MODULES=c_log.o data_array.o safe_client.o safe_file_exception.o safe_proxy.o safe_storage.o shared_safe_storage.o
MODULES=c_log.o data_array.o safe_file_exception.o safe_storage.o shared_safe_storage.o base_safe_file.o open_safe_storage.o safe_client.o factory.o
TARGET=../../lib/libsafestorage.so
all: $(TARGET)
$(TARGET): $(MODULES)
	$(CXX) $(CXXFLAGS) -shared $(MODULES) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $*.cpp -o $@

clean:
	rm *.o $(TARGET)
	

