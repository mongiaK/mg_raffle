#ifndef __bpf_map_def_h__
#define __bpf_map_def_h__

#include "root/usr/include/bpf/bpf_helpers.h"
#define BPF_EVENT(name, key_type, value_type, entries)                         \
    struct {                                                                   \
        __uint(type, BPF_MAP_TYPE_PERF_EVENT_ARRAY);                           \
        __uint(key_size, sizeof(key_type));                                    \
        __uint(value_size, sizeof(value_type));                                \
        __uint(max_entries, entries);                                          \
    } name SEC(".maps");

#define BPF_ARRAY(name, value_type, entries)                                   \
    struct {                                                                   \
        __uint(type, BPF_MAP_TYPE_ARRAY);                                      \
        __type(key, __u32);                                                    \
        __type(value, value_type);                                             \
        __uint(max_entries, entries);                                          \
    } name SEC(".maps");

#define BPF_HASH(name, key_type, value_type, entries)                          \
    struct {                                                                   \
        __uint(type, BPF_MAP_TYPE_HASH);                                       \
        __type(key, key_type);                                                 \
        __type(value, value_type);                                             \
        __uint(max_entries, entries);                                          \
    } name SEC(".maps");

#define BPF_TRACE(name, entries)                                               \
    struct {                                                                   \
        __uint(type, BPF_MAP_TYPE_STACK_TRACE);                                \
        __uint(key_size, sizeof(__u32));                                       \
        __uint(value_size, PERF_MAX_STACK_DEPTH * sizeof(__u64));              \
        __uint(max_entries, entries);                                          \
    } name SEC(".maps");

#define BPF_PERCPU_ARRAY(name, value_type, entries)                            \
    struct {                                                                   \
        __uint(type, BPF_MAP_TYPE_PERCPU_ARRAY);                               \
        __type(key, __u32);                                                    \
        __type(value, value_type);                                             \
        __uint(max_entries, entries);                                          \
    } name SEC(".maps");

#define BPF_RINGBUF(name, entries)                                             \
    struct {                                                                   \
        __uint(type, BPF_MAP_TYPE_RINGBUF);                                    \
        __uint(max_entries, entries);                                          \
    } name SEC(".maps");

#define BPF_LRU(name, key_type, value_type, entries)                           \
    struct {                                                                   \
        __uint(type, BPf_MAP_TYPE_LRU);                                        \
        __type(key, key_type);                                                 \
        __type(value, value_type);                                             \
        __uint(max_entries, entries);                                          \
    } name SEC(".maps");

#define BPF_QUEUE(name, value_type, entries)                                   \
    struct {                                                                   \
        __uint(type, BPf_MAP_TYPE_QUEUE);                                      \
        __type(value, value_type);                                             \
        __uint(max_entries, entries);                                          \
    } name SEC(".maps");

#define BPF_STACK(name, value_type, entries)                                   \
    struct {                                                                   \
        __uint(type, BPF_MAP_TYPE_STACK);                                      \
        __type(value, value_type);                                             \
        __uint(max_entries, entries);                                          \
    } name SEC(".maps");

#endif // !__bpf_map_def_h__
