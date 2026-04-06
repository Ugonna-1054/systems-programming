// class_dbg.cpp
#include <iostream>
#include <string>

class Position {
public:
    Position(std::string sym, int qty, double px)
        : symbol_(std::move(sym)), qty_(qty), avg_px_(px) {}

    void add_fill(int fill_qty, double fill_px) {
        const double notional = avg_px_ * qty_ + fill_px * fill_qty;
        qty_ += fill_qty;
        avg_px_ = (qty_ == 0) ? 0.0 : (notional / qty_);
    }

    int qty() const { return qty_; }
    double avg_px() const { return avg_px_; }

private:
    std::string symbol_;
    int qty_;
    double avg_px_;
};

int main() {
    Position pos("AAPL", 10, 100.0);
    pos.add_fill(5, 110.0);
    std::cout << "qty=" << pos.qty() << " avg=" << pos.avg_px() << "\n";
    return 0;
}
