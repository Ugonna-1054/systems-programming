## Compile loader
clang++ -O2 -g loader.cpp -o loader -lbpf

## Compile ebpf program
clang -O2 -g -target bpf -D__TARGET_ARCH_arm64 \
  -I/usr/include/aarch64-linux-gnu \
  -c openat.bpf.c -o openat.bpf.o
