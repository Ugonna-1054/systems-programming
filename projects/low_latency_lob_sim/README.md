# Low-Latency Limit Order Book Simulator (C++)

## V1 Scope
- In-memory limit order book with price-time matching.
- Synthetic order flow generator.
- Benchmark output for:
  - throughput (orders/s)
  - latency min/p50/p95/p99/max (ns)
  - trade/fill totals

## Build
If `cmake` is unavailable:

```bash
clang++ -std=c++20 -O3 -march=native -Wall -Wextra -Wpedantic \
  -Iinclude src/main.cpp src/order_book.cpp -o lob_bench
```

If `cmake` is available:

```bash
cmake -S . -B build
cmake --build build -j
./build/lob_bench 200000 42
```

## Run
```bash
./lob_bench <num_orders> <seed>
# example
./lob_bench 200000 42
```

## Next (V2)
- Per-stage latency breakdown (ingress, match, egress).
- Multi-threaded producer/consumer pipeline.
- Optional lock-free queue between stages.
- CSV/JSON metrics output for plotting.
