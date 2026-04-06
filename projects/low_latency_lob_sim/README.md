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

Optional CSV append output:

```bash
./lob_bench 200000 42 --csv benchmark_results.csv
```

CSV columns:
`orders,seed,elapsed_s,throughput_orders_per_s,resting_orders,total_trades,total_filled_qty,latency_ns_min,latency_ns_p50,latency_ns_p95,latency_ns_p99,latency_ns_max`

## Workload Matrix Runner
Run warmup + measured workload tiers (small/mid/large/xlarge):

```bash
chmod +x benchmark.sh
./benchmark.sh
```

This writes `benchmark_results.csv` with one row per run and prefixed workload metadata.

## Next (V2)
- Per-stage latency breakdown (ingress, match, egress).
- Multi-threaded producer/consumer pipeline.
- Optional lock-free queue between stages.
- Perf-based hotspot profiling and before/after comparison.
