MODULES=c_log.o data_array.o safe_file_exception.o safe_storage.o shared_safe_storage.o base_safe_file.o open_safe_storage.o\
 safe_client.o factory.o crc32.o safe_storage_listener.o safe_storage_replica.o
 
TARGET=$(LIBDIR)/libsafestorage.so

include ../Makefile.defaults

$(TARGET): $(MODULES)
	$(CXX) $(CXXFLAGS) -shared $(MODULES) -o $@

crc32.o: crc32.cpp
	$(CXX) $(CXXFLAGS) -O3 -c $*.cpp -o $@
