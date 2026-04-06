#include "orderbook/order_book.h"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>

namespace {

struct BenchConfig {
  uint64_t orders = 500000;
  uint32_t seed = 42;
};

BenchConfig parse_args(int argc, char** argv) {
  BenchConfig cfg;

  if (argc >= 2) {
    cfg.orders = static_cast<uint64_t>(std::strtoull(argv[1], nullptr, 10));
  }
  if (argc >= 3) {
    cfg.seed = static_cast<uint32_t>(std::strtoul(argv[2], nullptr, 10));
  }

  return cfg;
}

uint64_t percentile_ns(std::vector<uint64_t>& samples, double pct) {
  if (samples.empty()) {
    return 0;
  }

  const double rank = (pct / 100.0) * static_cast<double>(samples.size() - 1);
  const auto idx = static_cast<size_t>(rank);
  std::nth_element(samples.begin(), samples.begin() + idx, samples.end());
  return samples[idx];
}

}  // namespace

int main(int argc, char** argv) {
  const BenchConfig cfg = parse_args(argc, argv);

  lob::OrderBook book;
  std::vector<uint64_t> latencies_ns;
  latencies_ns.reserve(static_cast<size_t>(cfg.orders));

  std::mt19937_64 rng(cfg.seed);
  std::uniform_int_distribution<int> side_dist(0, 1);
  std::uniform_int_distribution<int64_t> price_offset(-50, 50);
  std::uniform_int_distribution<uint32_t> qty_dist(1, 200);

  uint64_t next_id = 1;
  uint64_t total_trades = 0;
  uint64_t total_filled_qty = 0;
  constexpr int64_t base_price = 10000;

  const auto bench_start = std::chrono::steady_clock::now();

  for (uint64_t i = 0; i < cfg.orders; ++i) {
    lob::Order order{
        .id = next_id++,
        .side = side_dist(rng) == 0 ? lob::Side::Buy : lob::Side::Sell,
        .price_ticks = base_price + price_offset(rng),
        .qty = qty_dist(rng),
        .ts_ns = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(
                                    std::chrono::steady_clock::now().time_since_epoch())
                                    .count()),
    };

    const auto t0 = std::chrono::steady_clock::now();
    const lob::MatchStats stats = book.add(order);
    const auto t1 = std::chrono::steady_clock::now();

    const uint64_t ns = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count());
    latencies_ns.push_back(ns);

    total_trades += stats.trades;
    total_filled_qty += stats.filled_qty;
  }

  const auto bench_end = std::chrono::steady_clock::now();
  const double elapsed_s =
      std::chrono::duration_cast<std::chrono::duration<double>>(bench_end - bench_start).count();

  std::vector<uint64_t> p50_buf = latencies_ns;
  std::vector<uint64_t> p95_buf = latencies_ns;
  std::vector<uint64_t> p99_buf = latencies_ns;
  const uint64_t p50 = percentile_ns(p50_buf, 50.0);
  const uint64_t p95 = percentile_ns(p95_buf, 95.0);
  const uint64_t p99 = percentile_ns(p99_buf, 99.0);

  const auto minmax = std::minmax_element(latencies_ns.begin(), latencies_ns.end());
  const double throughput = elapsed_s > 0.0 ? static_cast<double>(cfg.orders) / elapsed_s : 0.0;

  std::cout << "LOB Benchmark Results\n";
  std::cout << "orders: " << cfg.orders << "\n";
  std::cout << "seed: " << cfg.seed << "\n";
  std::cout << "elapsed_s: " << std::fixed << std::setprecision(6) << elapsed_s << "\n";
  std::cout << "throughput_orders_per_s: " << std::fixed << std::setprecision(2) << throughput
            << "\n";
  std::cout << "resting_orders: " << book.total_resting_orders() << "\n";
  std::cout << "total_trades: " << total_trades << "\n";
  std::cout << "total_filled_qty: " << total_filled_qty << "\n";
  std::cout << "latency_ns_min: " << *minmax.first << "\n";
  std::cout << "latency_ns_p50: " << p50 << "\n";
  std::cout << "latency_ns_p95: " << p95 << "\n";
  std::cout << "latency_ns_p99: " << p99 << "\n";
  std::cout << "latency_ns_max: " << *minmax.second << "\n";

  return 0;
}
