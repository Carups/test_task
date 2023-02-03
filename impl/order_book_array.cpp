#include <array>

#include "order_book.h"

struct order_book::impl {
    static constexpr uint32_t array_size = 30;
    std::array<snapshot::data, array_size> asks{};
    std::array<snapshot::data, array_size> bids{};
    size_t end_asks = 0;
    size_t end_bids = 0;
    snapshot::data best_ask{-1, -1};
    snapshot::data best_bid{-1, -1};
    void update_bests() {
        if (end_asks > 0) {
            best_ask = asks[0];
            for (size_t i = 1; i < end_asks; ++i) {
                if (best_ask.price > asks[i].price) {
                    best_ask = asks[i];
                }
            }
        } else {
            best_ask = {-1, -1};
        }

        if (end_bids > 0) {
            best_bid = bids[0];
            for (size_t i = 1; i < end_bids; ++i) {
                if (best_bid.price < bids[i].price) {
                    best_bid = bids[i];
                }
            }
        } else {
            best_bid = {-1, -1};
        }
    }

    explicit impl(const snapshot &init) {
        for(const auto &node : init.asks) {
           asks[end_asks++] = node;
        }
        for(const auto &node : init.bids) {
            bids[end_bids++] = node;
        }
    }

    void update(const snapshot &update) {
        auto update_ =
            [](std::array<snapshot::data, array_size>& arr, size_t &end, const snapshot::data& node) {
            for (size_t i = 0; i < end; ++i) {
                if (fabs(node.price - arr[i].price) < EPS) {
                    if (fabs(node.amount) < EPS) {
                        std::swap(arr[i], arr[end - 1]);
                        end--;
                    } else {
                        arr[i].amount = node.amount;
                    }
                    return;
                }
            }
            arr[end++] = node;
        };

        for(const auto &node : update.asks) {
            update_(asks, end_asks, node);
        }
        for(const auto &node : update.bids){
            update_(bids, end_bids, node);
        }
    }
};

order_book::order_book(const snapshot &init) {
    impl_ = std::make_unique<impl>(init);
}

order_book::~order_book() = default;

void order_book::update(const snapshot &update) {
    impl_->update(update);
}

std::tuple<double, double, double, double> order_book::get_tops() {
    impl_->update_bests();
    return {impl_->best_ask.price, impl_->best_ask.amount, impl_->best_bid.price, impl_->best_bid.amount};
}