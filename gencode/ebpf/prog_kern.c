#include <linux/bpf.h>
#include <linux/ptrace.h>

#include "root/usr/include/bpf/bpf_helpers.h"
#include "root/usr/include/bpf/bpf_tracing.h"

#include "bpf_map_def.h"

BPF_EVENT(events_demo, __u32, __u32, 1024)

BPF_ARRAY(array_demo, __u64, 256)

SEC("kprobe/tcp_v4_connect")
int bpf_prog(struct pt_regs *ctx) {
    struct sock *sk = (struct sock *)PT_REGS_PARM1(ctx);
    bpf_printk("mongia hook tcp connect\n");
    return 0;
}

char __license[] SEC("license") = "GPL";
