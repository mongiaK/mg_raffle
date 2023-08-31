target:=ebpf

CXX:=clang

CFLAGS=-g -Wall -O0 -D__TARGET_ARCH_x86

LIB_INCS= #-L/home/mongia/libbpf/src
STATIC_LIBS=-lbpf 
DYNAMIC_LIBS=-lrt -ldl -lpthread -lm -lz -lelf

#VMLINUX := /home/mongia/libbpf-bootstrap/vmlinux/x86/vmlinux.h

OBJ_DIR=obj

INCS := -I$(OBJ_DIR) 
#-I$(dir $(VMLINUX))
DIRS := $(shell find . -maxdepth 5 -type d)

SRC=$(foreach dir, $(DIRS), $(wildcard $(dir)/*.c))
BPF_SRC=$(filter %.bpf.c, $(SRC))
USER_SRC=$(filter-out %.bpf.c, $(SRC))

BPF_OBJ=$(addprefix $(OBJ_DIR)/,$(patsubst %.c,%.o,$(notdir $(BPF_SRC))))
USER_OBJ=$(addprefix $(OBJ_DIR)/,$(patsubst %.c,%.o,$(notdir $(USER_SRC))))
BPF_SKEL_H=$(patsubst %.bpf.o,%.skel.h,$(BPF_OBJ))

all: prepare ${BPF_OBJ} ${BPF_SKEL_H} $(target) 

prepare:
	@mkdir -p ${OBJ_DIR}

$(target): ${USER_OBJ} 
	${CXX} ${CFLAGS} ${INCS} $^ -o $@ ${LIB_INCS} -Wl,-Bstatic ${STATIC_LIBS} -Wl,-Bdynamic ${DYNAMIC_LIBS}

# build bpf code
${OBJ_DIR}/%.bpf.o: %.bpf.c 
	${CXX} ${CFLAGS} -target bpf ${INCS} -c $^ -o $@

#build skel.h
${OBJ_DIR}/%.skel.h: ${OBJ_DIR}/%.bpf.o
	@bpftool gen skeleton $< > $@

#build userspace code
$(OBJ_DIR)/%.o: %.c
	${CXX} ${CFLAGS} ${INCS} -c $< -o $@

.PHONY: clean

clean:
	@rm -rf ${OBJ_DIR} ${target}
