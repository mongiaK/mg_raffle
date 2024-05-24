#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include "root/usr/include/bpf/libbpf.h"

static volatile sig_atomic_t stop;

static int libbpf_print_fn(enum libbpf_print_level level, const char *format,
                           va_list args) {
    return vfprintf(stderr, format, args);
}

static void sig_int(int signo) { stop = 1; }

int main(int argc, char **argv) {
    struct bpf_object *obj;
    int err;

    libbpf_set_print(libbpf_print_fn);

    // 加载 BPF 程序对象
    obj = bpf_object__open("prog_kern.o");
    if (NULL == obj) {
        fprintf(stderr, "ERROR: opening BPF object file failed\n");
        return 1;
    }

    // 加载 BPF 程序到内核
    err = bpf_object__load(obj);
    if (err) {
        fprintf(stderr, "ERROR: loading BPF object file failed\n");
        goto cleanup;
    }

    struct bpf_program *prog;
    bpf_object__for_each_program(prog, obj) {
        struct bpf_link *link = bpf_program__attach(prog);
        if (link == NULL) {
            fprintf(stderr, "ERROR: attaching BPF program failed\n");
            goto cleanup;
        }
    }

    if (signal(SIGINT, sig_int) == SIG_ERR) {
        fprintf(stderr, "can't set signal handler: %s\n", strerror(errno));
        goto cleanup;
    }

    while (!stop) {
        fprintf(stderr, ".");
        sleep(1);
    }

cleanup:
    // 清理并关闭 BPF 程序对象
    bpf_object__close(obj);
    return err != 0;
}
