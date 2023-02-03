#include <unordered_map>

#include "order_book.h"

struct order_book::impl {
    std::unordered_map<double, double> asks_;
    std::unordered_map<double, double> bids_;
    explicit impl(const snapshot &init) {
        for (const auto& node: init.asks) {
            asks_.insert({node.price, node.amount});
        }
        for (const auto& node: init.bids) {
            bids_.insert({node.price, node.amount});
        }
    }

    snapshot::data best_ask{-1, -1};
    snapshot::data best_bid{-1, -1};
    void update_bests() {
        if (!asks_.empty()) {
            best_ask = {asks_.begin()->first, asks_.begin()->second};
            for (auto it : asks_) {
                if (best_ask.price > it.first) {
                    best_ask = {it.first, it.second};
                }
            }
        } else {
            best_ask = {-1, -1};
        }

        if (!bids_.empty()) {
            best_bid = {bids_.begin()->first, bids_.begin()->second};;
            for (auto it : bids_) {
                if (best_bid.price < it.first) {
                    best_bid = {it.first, it.second};
                }
            }
        } else {
            best_bid = {-1, -1};
        }
    }
};

order_book::order_book(const snapshot &init) {
    impl_ = std::make_unique<impl>(init);
}

void order_book::update(const snapshot &update) {
    auto update_ = [](auto &map_, const snapshot::data &node) {
        if (fabs(node.amount) < EPS) {
            map_.erase(node.price);
        } else {
            auto it = map_.insert({node.price, node.amount});
            if (!it.second) {
                it.first->second = node.amount;
            }
        }
    };

    for (const auto& node: update.asks) {
        update_(impl_->asks_, node);
    }
    for (const auto& node: update.bids) {
        update_(impl_->bids_, node);
    }
}

order_book::~order_book() = default;

std::tuple<double, double, double, double> order_book::get_tops() {
    impl_->update_bests();
    return {impl_->best_ask.price, impl_->best_ask.amount, impl_->best_bid.price, impl_->best_bid.amount};
}
