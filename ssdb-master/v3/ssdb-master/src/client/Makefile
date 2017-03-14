include ../../build_config.mk

all: lib
	${CXX} -g -o hello-ssdb hello-ssdb.cpp libssdb-client.a ${CLIBS}

lib: SSDB_client.h SSDB_impl.h SSDB_impl.cpp
	${CXX} -I../ ${CFLAGS} -c SSDB_impl.cpp 
	ar -cru libssdb-client.a\
		SSDB_impl.o\
		../util/bytes.o\
		../net/link.o \
		../util/log.o \
		../request_example.pb.o \
		../response_example.pb.o

clean:
	rm -rf hello-ssdb *.a *.o 

