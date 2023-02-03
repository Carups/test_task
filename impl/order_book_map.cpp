#include <map>

#include "order_book.h"

struct order_book::impl {
    std::map<double, double> asks_;
    std::map<double, double> bids_;
    explicit impl(const snapshot &init) {
        for (const auto& node: init.asks) {
            asks_.insert({node.price, node.amount});
        }
        for (const auto& node: init.bids) {
            bids_.insert({node.price, node.amount});
        }
    }
};

order_book::order_book(const snapshot &init) {
    impl_ = std::make_unique<impl>(init);
}

void order_book::update(const snapshot &update) {
    auto update_ = [](std::map<double, double> &map_, const snapshot::data &node) {
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
    auto bid = impl_->asks_.begin();
    auto ask = impl_->bids_.rbegin();
    return {bid->first, bid->second, ask->first, ask->second};
}
