#pragma once

#include <cstdint>
#include <deque>
#include <map>

namespace lob {

enum class Side : uint8_t {
  Buy = 0,
  Sell = 1,
};

struct Order {
  uint64_t id;
  int64_t price_ticks;
  uint64_t ts_ns;
  uint32_t qty;
  Side side;
};

struct MatchStats {
  uint32_t filled_qty;
  uint32_t remaining_qty;
  uint32_t trades;
};

class OrderBook {
 public:
  MatchStats add(Order order);

  [[nodiscard]] uint64_t total_resting_orders() const;

 private:
  using LevelQueue = std::deque<Order>;

  std::map<int64_t, LevelQueue, std::greater<>> bids_;
  std::map<int64_t, LevelQueue, std::less<>> asks_;

  static void match_at_level(LevelQueue& level, uint32_t& qty, uint32_t& trades);
};

}  // namespace lob
