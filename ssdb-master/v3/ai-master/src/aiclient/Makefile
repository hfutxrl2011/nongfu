include ../../build_config.mk

all: lib
	${CXX} -g -o hello-ai hello-ai.cpp libai-client.a ${CLIBS}

lib: AI_client.h AI_impl.h AI_impl.cpp game.pb.cc
	${CXX} -I../ ${CFLAGS} -c AI_impl.cpp 
	${CXX} -I../ ${CFLAGS} -c game.pb.cc
	ar -cru libai-client.a\
		AI_impl.o\
		../net/link.o \
		../util/log.o \
		./game.pb.o

clean:
	rm -rf hello-ai *.a *.o 

