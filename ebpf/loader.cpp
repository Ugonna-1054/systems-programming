#include <bpf/libbpf.h>
#include <csignal>
#include <cstdio>
#include <cstdlib>

static volatile sig_atomic_t stop_flag = 0;

struct event {
  uint32_t pid;
  char comm[16];
};

static void on_sigint(int) { stop_flag = 1; }

static int handle_event(void *ctx, void *data, size_t) {
  FILE *out = static_cast<FILE*>(ctx);
  const event *e = static_cast<const event*>(data);
  std::fprintf(out, "pid=%u comm=%s\n", e->pid, e->comm);
  std::fflush(out);
  return 0;
}

int main() {
  std::signal(SIGINT, on_sigint);
  std::signal(SIGTERM, on_sigint);

  FILE *out = std::fopen("events.log", "w");
  if (!out) return 1;

  bpf_object *obj = bpf_object__open_file("openat.bpf.o", nullptr);
  if (!obj) return 1;

    bpf_program *p = bpf_object__find_program_by_name(obj, "myCustomSec");
    bpf_program__set_autoload(p, false);

  if (bpf_object__load(obj)) return 1;

  bpf_program *prog = bpf_object__find_program_by_name(obj, "on_openat");
  if (!prog) return 1;

  bpf_link *link = bpf_program__attach_tracepoint(prog, "syscalls", "sys_enter_openat");
  if (!link) return 1;


  bpf_map *map = bpf_object__find_map_by_name(obj, "events");
  if (!map) return 1;

  int map_fd = bpf_map__fd(map);
  ring_buffer *rb = ring_buffer__new(map_fd, handle_event, out, nullptr);
  if (!rb) return 1;

  while (!stop_flag) ring_buffer__poll(rb, 100);

  ring_buffer__free(rb);
  bpf_link__destroy(link);
  bpf_object__close(obj);
  std::fclose(out);
  return 0;
}
