#include "orderbook/order_book.h"

namespace lob {

void OrderBook::match_at_level(LevelQueue& level, uint32_t& qty, uint32_t& trades) {
  while (qty > 0 && !level.empty()) {
    Order& top = level.front();
    const uint32_t traded = (top.qty < qty) ? top.qty : qty;
    top.qty -= traded;
    qty -= traded;
    ++trades;

    if (top.qty == 0) {
      level.pop_front();
    }
  }
}

MatchStats OrderBook::add(Order order) {
  const uint32_t original_qty = order.qty;
  uint32_t trades = 0;

  if (order.side == Side::Buy) {
    while (order.qty > 0 && !asks_.empty()) {
      auto best_ask_it = asks_.begin();
      if (best_ask_it->first > order.price_ticks) {
        break;
      }

      match_at_level(best_ask_it->second, order.qty, trades);
      if (best_ask_it->second.empty()) {
        asks_.erase(best_ask_it);
      }
    }

    if (order.qty > 0) {
      bids_[order.price_ticks].push_back(order);
    }
  } else {
    while (order.qty > 0 && !bids_.empty()) {
      auto best_bid_it = bids_.begin();
      if (best_bid_it->first < order.price_ticks) {
        break;
      }

      match_at_level(best_bid_it->second, order.qty, trades);
      if (best_bid_it->second.empty()) {
        bids_.erase(best_bid_it);
      }
    }

    if (order.qty > 0) {
      asks_[order.price_ticks].push_back(order);
    }
  }

  return MatchStats{
      .filled_qty = static_cast<uint32_t>(original_qty - order.qty),
      .remaining_qty = order.qty,
      .trades = trades,
  };
}

uint64_t OrderBook::total_resting_orders() const {
  uint64_t total = 0;

  for (const auto& [price, level] : bids_) {
    (void)price;
    total += level.size();
  }

  for (const auto& [price, level] : asks_) {
    (void)price;
    total += level.size();
  }

  return total;
}

}  // namespace lob
