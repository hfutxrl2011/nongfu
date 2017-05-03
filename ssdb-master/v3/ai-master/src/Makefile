include ../build_config.mk

OBJS = proc_kv.o serv.o ai-server.o response_example.pb.o request_example.pb.o client/game.pb.o
LIBS = ./net/libnet.a ./util/libutil.a 
EXES = ai-server


all: ${OBJS}
	make -C net
	make -C util
	#make -C client
	${CXX} -o ${EXES} ${OBJS} ${LIBS} ${CLIBS}
	cp -rf ${EXES} ../
	cp -rf ai.conf ../

ai-server.o: ai-server.cpp
	${CXX} ${CFLAGS} -c ai-server.cpp

proc_kv.o: serv.h proc_kv.cpp
	${CXX} ${CFLAGS} -c proc_kv.cpp

response_example.pb.o: response_example.pb.h response_example.pb.cc
	${CXX} ${CFLAGS} -c response_example.pb.cc

request_example.pb.o: request_example.pb.h request_example.pb.cc
	${CXX} ${CFLAGS} -c request_example.pb.cc

client/game.pb.o: client/game.pb.h client/game.pb.cc
	${CXX} ${CFLAGS} -c client/game.pb.cc

serv.o: serv.h serv.cpp
	${CXX} ${CFLAGS} -c serv.cpp

clean:
	rm -f ${EXES} *.o *.exe *.a ../${EXES} ../ai.conf
	make clean -C net
	make clean -C util
	#make clean -C client
