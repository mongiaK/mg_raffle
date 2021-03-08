CXX:=clang++

INCS=-I ../utils

CFLAGS=-g -Wall -O0 -std=c++11
LIBS=-lpthread

SRC=${wildcard *.cpp}
OBJ=${patsubst %.cpp,%.o,${SRC}}

.PHONY: clean

all: music_tools

#mg_tools: mg_tools.o
#	${CXX} ${CFLAGS} $^ -o $@ ${LIBS}
#
#multi: multi.o
#	${CXX} ${CFLAGS} $^ -o $@ ${LIBS}
#
#tserver: tserver.o
#	${CXX} ${CFLAGS} $^ -o $@ ${LIBS}

music_tools: music_change_name.o
	${CXX} ${CFLAGS} ${INCS} $^ -o $@ ${LIBS}

%.o:%.cpp
	${CXX} ${CFLAGS} ${INCS} -c $< -o $@

clean:
	@rm -f ${OBJ} music_tools
