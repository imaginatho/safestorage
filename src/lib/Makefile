SOURCES := $(wildcard *.cpp) $(wildcard ../base/*.cpp)
 
TARGET=$(LIBDIR)/libsafestorage.so

include ../Makefile.defaults

$(TARGET): $(MODULES)
	$(CXX) $(CXXFLAGS) -shared $(MODULES) -o $@ -lpthread

crc32.o: crc32.cpp
	$(CXX) $(CXXFLAGS) -O3 -c $*.cpp -o $@
