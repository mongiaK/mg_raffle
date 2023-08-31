target:=m

CXX:=g++

INCS=-I/home/mongia/protobuf-3.20.0/src
CFLAGS=-g -Wall -O0 -std=c++14
LIBS=-L/home/mongia/protobuf-3.20.0/src/.libs
STATIC_LIBS=-lprotobuf
DYNAMIC_LIBS=-lm -lc
OBJ_DIR=obj

DIRS := $(shell find . -maxdepth 5 -type d) 
VPATH = ${DIRS}

SRC=$(foreach dir, $(DIRS), $(wildcard $(dir)/*.cpp))
OBJ=$(addprefix $(OBJ_DIR)/,$(patsubst %.cpp,%.o,$(SRC)))

all: prepare $(target)

prepare:
	@mkdir -p ${OBJ_DIR}

$(target): ${OBJ}
	${CXX} ${CFLAGS} ${INCS} $^ -o $@ ${LIBS} -Wl,-Bstatic ${STATIC_LIBS} -Wl,-Bdynamic ${DYNAMIC_LIBS}

$(OBJ_DIR)/%.o: %.cpp
	${CXX} ${CFLAGS} ${INCS} -c $< -o $@

.PHONY: clean

clean:
	@rm -rf ${OBJ_DIR} ${target}
