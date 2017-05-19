include ../../build_config.mk

all: mylib
	${CXX} -g -o api-server api-server.cpp libai-client.a ./lib/pbjson/src/pbjson.hpp -I./lib/pbjson/src -I/usr/include/python2.6 -lpython2.6  ${CLIBS} 
	${CXX} -g -o hello-ai hello-ai.cpp libai-client.a ./lib/pbjson/src/pbjson.hpp -I./lib/pbjson/src -I/usr/include/python2.6 -lpython2.6 ${CLIBS} 

mylib: AI_client.h AI_impl.h AI_impl.cpp game.pb.cc cepollclient.h cepollclient.cpp
	${CXX} -I../ ${CFLAGS} -c game.pb.cc
	${CXX} -I../ ${CFLAGS} -c actions.pb.cc
	${CXX} -I../ -I./lib/pbjson/src -I/usr/include/python2.6 -lpython2.6 ${CFLAGS} -c cepollclient.cpp
	${CXX} -I./lib/pbjson/src/ ${CFLAGS} -c ./lib/pbjson/src/pbjson.cpp
	${CXX} -I../ -I./lib/pbjson/src ${CFLAGS} -c AI_impl.cpp 
	ar -cru libai-client.a\
		AI_impl.o\
		../net/link.o \
		../util/log.o \
		./lib/pbjson/src/pbjson.o \
		./actions.pb.o \
		./cepollclient.o \
		./game.pb.o

clean:
	rm -rf api-server *.a *.o 

