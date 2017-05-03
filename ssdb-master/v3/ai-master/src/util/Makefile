include ../../build_config.mk

OBJS = log.o config.o app.o thread_mem.o signal.o
EXES = 

all: ${OBJS}
	ar -cru ./libutil.a ${OBJS}

app.o: app.h app.cpp
	${CXX} ${CFLAGS} -c app.cpp

log.o: log.h log.cpp
	${CXX} ${CFLAGS} -c log.cpp

config.o: config.h config.cpp
	${CXX} ${CFLAGS} -c config.cpp

thread_mem.o: thread_mem.h thread_mem.cpp
	${CXX} ${CFLAGS} -c thread_mem.cpp

signal.o: signal.h signal.cpp
	${CXX} ${CFLAGS} -c signal.cpp

test:
	$(CXX) ${CFLAGS} test_sorted_set.cpp $(OBJS)

clean:
	rm -f ${EXES} ${OBJS} *.o *.exe *.a

