#include <linux/bpf.h>
#include <linux/ptrace.h>

#include "root/usr/include/bpf/bpf_helpers.h"
#include "root/usr/include/bpf/bpf_tracing.h"

SEC("kprobe/tcp_v4_connect")
int bpf_prog(struct pt_regs *ctx) {
    struct sock *sk = (struct sock *)PT_REGS_PARM1(ctx);
    bpf_printk("mongia hook tcp connect\n");
    return 0;
}

char __license[] SEC("license") = "GPL";
