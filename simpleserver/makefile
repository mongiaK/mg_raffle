CXX:=clang++

INCS=-I ../utils

CFLAGS=-g -Wall -O0 -std=c++11
LIBS=-lpthread

SRC=${wildcard *.cpp}
OBJ=${patsubst %.cpp,%.o,${SRC}}

.PHONY: clean

all: simpleserver

simpleserver: tserver.o
	${CXX} ${CFLAGS} ${INCS} $^ -o $@ ${LIBS}

%.o:%.cpp
	${CXX} ${CFLAGS} ${INCS} -c $< -o $@

clean:
	@rm -f ${OBJ} simpleserver
