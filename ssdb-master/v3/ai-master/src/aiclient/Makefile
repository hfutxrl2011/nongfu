include ../../build_config.mk

all: mylib
	${CXX} -g -o api-server api-server.cpp libai-client.a ThreadPool.h ThreadPool.cpp ./lib/pbjson/src/pbjson.hpp -I./lib/pbjson/src -I/usr/include/python2.6 -lpython2.6  ${CLIBS} 
	mkdir -p ../../../bin && cp api-server ../../../bin && cp *.py ../../../bin && rm -rf static/*.log && cp -r static ../../../bin
	${CXX} -g -o hello-ai hello-ai.cpp libai-client.a ./lib/pbjson/src/pbjson.hpp -I./lib/pbjson/src -I/usr/include/python2.6 -lpython2.6 ${CLIBS} 

mylib: AI_client.h AI_impl.h AI_impl.cpp game.pb.cc ThreadPool.h cepollclient.h ../net/link.h ../util/log.h
	${CXX} -I../net ${CFLAGS} -c ../net/link.cpp
	${CXX} -I../util ${CFLAGS} -c ../util/log.cpp
	${CXX} -I../ ${CFLAGS} -c game.pb.cc
	${CXX} -I../ ${CFLAGS} -c actions.pb.cc
	${CXX} -I./ ${CFLAGS} -I/usr/include/python2.6 -lpython2.6 -c ThreadPool.cpp
	${CXX} -I./lib/pbjson/src/ ${CFLAGS} -c ./lib/pbjson/src/pbjson.cpp
	${CXX} -I../ -I./lib/pbjson/src ${CFLAGS} -c AI_impl.cpp
	${CXX} -I../ -I./lib/pbjson/src ${CFLAGS} -I/usr/include/python2.6 -lpython2.6 -c cepollclient.cpp
	ar -cru libai-client.a\
		AI_impl.o\
		./link.o \
		./log.o \
		./pbjson.o \
		./actions.pb.o \
		./cepollclient.o \
		./game.pb.o

clean:
	rm -rf api-server *.a *.o 

