#include <order_book.h>

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>

namespace {

struct BenchConfig {
  uint64_t orders = 1000000;
  uint32_t seed = 42;
  std::string csv_path;
};

struct BenchResult {
  double elapsed_s;
  double throughput;
  uint64_t resting_orders;
  uint64_t total_trades;
  uint64_t total_filled_qty;
  uint64_t latency_min;
  uint64_t latency_p50;
  uint64_t latency_p95;
  uint64_t latency_p99;
  uint64_t latency_p999;
  uint64_t latency_max;
};

BenchConfig parse_args(int argc, char** argv) {
  BenchConfig cfg;

  int positional_idx = 0;
  for (int i = 1; i < argc; ++i) {
    const std::string arg = argv[i];
    if (arg == "--csv") {
      if (i + 1 >= argc) {
        std::cerr << "missing path after --csv\n";
        std::exit(2);
      }
      cfg.csv_path = argv[++i];
      continue;
    }

    if (positional_idx == 0) {
      cfg.orders = static_cast<uint64_t>(std::strtoull(arg.c_str(), nullptr, 10));
    } else if (positional_idx == 1) {
      cfg.seed = static_cast<uint32_t>(std::strtoul(arg.c_str(), nullptr, 10));
    } else {
      std::cerr << "unexpected argument: " << arg << "\n";
      std::exit(2);
    }
    ++positional_idx;
  }

  return cfg;
}

uint64_t percentile_ns(const std::vector<uint64_t>& samples, double pct) {
  if (samples.empty()) {
    return 0;
  }

  const double rank = (pct / 100.0) * static_cast<double>(samples.size() - 1);
  const auto idx = static_cast<size_t>(rank);
  return samples[idx];
}

void append_csv_row(const BenchConfig& cfg, const BenchResult& result) {
  if (cfg.csv_path.empty()) {
    return;
  }

  std::ofstream out(cfg.csv_path, std::ios::app);
  if (!out) {
    std::cerr << "failed to open csv file: " << cfg.csv_path << "\n";
    std::exit(3);
  }

  out << cfg.orders << ',' << cfg.seed << ',' << std::fixed << std::setprecision(6)
      << result.elapsed_s << ',' << std::setprecision(2) << result.throughput << ','
      << result.resting_orders << ',' << result.total_trades << ',' << result.total_filled_qty
      << ',' << result.latency_min << ',' << result.latency_p50 << ',' << result.latency_p95
      << ',' << result.latency_p99 << ',' << result.latency_max << '\n';
}

void print_human_output(const BenchConfig& cfg, const BenchResult& result) {
  std::cout << "LOB Benchmark Results\n";
  std::cout << "orders: " << cfg.orders << "\n";
  std::cout << "seed: " << cfg.seed << "\n";
  std::cout << "elapsed_s: " << std::fixed << std::setprecision(6) << result.elapsed_s << "\n";
  std::cout << "throughput_orders_per_s: " << std::fixed << std::setprecision(2)
            << result.throughput << "\n";
  std::cout << "resting_orders: " << result.resting_orders << "\n";
  std::cout << "total_trades: " << result.total_trades << "\n";
  std::cout << "total_filled_qty: " << result.total_filled_qty << "\n";
  std::cout << "latency_ns_min: " << result.latency_min << "\n";
  std::cout << "latency_ns_p50: " << result.latency_p50 << "\n";
  std::cout << "latency_ns_p95: " << result.latency_p95 << "\n";
  std::cout << "latency_ns_p99: " << result.latency_p99 << "\n";
  std::cout << "latency_ns_p999: " << result.latency_p999 << "\n";
  std::cout << "latency_ns_max: " << result.latency_max << "\n";
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

  std::cout << "sizeof Order: " << sizeof(lob::Order) << std::endl;
  std::cout << "sizeof LevelQueue: " << sizeof(std::deque<lob::Order>) << std::endl;

  for (uint64_t i = 0; i < cfg.orders; ++i) {
    lob::Order order{
        .id = next_id++,
        .price_ticks = base_price + price_offset(rng),
        .ts_ns = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(
                                    std::chrono::steady_clock::now().time_since_epoch())
                                    .count()),
        .qty = qty_dist(rng),
        .side = side_dist(rng) == 0 ? lob::Side::Buy : lob::Side::Sell,
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

  const double throughput = elapsed_s > 0.0 ? static_cast<double>(cfg.orders) / elapsed_s : 0.0;

  std::ranges::sort(latencies_ns);

  const BenchResult result{
      .elapsed_s = elapsed_s,
      .throughput = throughput,
      .resting_orders = book.total_resting_orders(),
      .total_trades = total_trades,
      .total_filled_qty = total_filled_qty,
      .latency_min = latencies_ns.front(),
      .latency_p50 = percentile_ns(latencies_ns, 50.0),
      .latency_p95 = percentile_ns(latencies_ns, 95.0),
      .latency_p99 = percentile_ns(latencies_ns, 99.0),
      .latency_p999 = percentile_ns(latencies_ns, 99.9),
      .latency_max = latencies_ns.back()
  };

  print_human_output(cfg, result);
  append_csv_row(cfg, result);

  return 0;
}
