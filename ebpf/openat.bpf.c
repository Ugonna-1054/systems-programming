#include <linux/bpf.h>
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>

struct event {
    __u32 pid;
    char comm[16];
};

struct {
    __uint(type, BPF_MAP_TYPE_RINGBUF);
    __uint(max_entries, 1 << 20); // 1MB
} events SEC(".maps");

SEC("tracepoint/syscalls/sys_enter_openat")
int on_openat(void *ctx) {

    char comm[16];
    if (bpf_get_current_comm(comm, sizeof(comm)) != 0) {
        return 0;
    }

    if (!(comm[0] == 'c' && comm[1] == 'p' && comm[2] == 'u')) {
        return 0;
    }

    struct event *e = bpf_ringbuf_reserve(&events, sizeof(*e), 0);
    if (!e) return 0;

    e->pid = (__u32)(bpf_get_current_pid_tgid() >> 32);
    for(int i=0; i<sizeof(e->comm); ++i) {
        e->comm[i]=comm[i];
    }

    bpf_ringbuf_submit(e, 0);
    return 0;
}

SEC("ugo/custom")
int myCustomSec () {
    return 0;
}

char LICENSE[] SEC("license") = "GPL";
