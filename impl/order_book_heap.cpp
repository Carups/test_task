#include <vector>

#include "order_book.h"

struct order_book::impl {
    template<typename Comp>
    class heap {
    private:
        bool upscale(size_t pos) {
            if (pos == 1) {
                return false;
            }
            size_t parent_pos = (pos >> 1) - 1;
            pos--;
            if (comp_(base_[parent_pos], base_[pos])) {
                return false;
            }
            std::swap(base_[parent_pos], base_[pos]);
            return true;
        }

        size_t downscale(size_t pos) {
            size_t left = 2 * pos - 1;
            size_t right = 2 * pos;
            pos--;

            if (right >= base_.size()) {
                if (left < base_.size() && !comp_(base_[pos], base_[left])) {
                    std::swap(base_[pos], base_[left]);
                    return left + 1;
                }
            } else if (!(comp_(base_[pos], base_[left])) || !(comp_(base_[pos], base_[right]))) {
                if (comp_(base_[left], base_[right])) {
                    std::swap(base_[pos], base_[left]);
                    return left + 1;
                } else {
                    std::swap(base_[pos], base_[right]);
                    return right + 1;
                }
            }
            return -1;
        }

        void push(const snapshot::data& node) {
            base_.emplace_back(node);
            size_t pos = base_.size();
            while(upscale(pos)) {
                pos >>= 1;
            }
        }
        void remove(int pos) {
            if (pos + 1 == base_.size()) {
                base_.pop_back();
                return;
            }
            std::swap(base_[pos], base_.back());
            base_.pop_back();
            pos++;
            while(upscale(pos)) {
                pos >>= 1;
            }
            do {
                pos = downscale(pos);
            }
            while(pos != -1);

        }
    public:
        void build(const std::vector<snapshot::data> &init) {
            base_ = init;
        }
        snapshot::data top() {
            if (base_.empty()) {
                return {-1, -1};
            }
            return base_[0];
        }
        void update(const snapshot::data& node) {
            for (size_t i = 0; i < base_.size(); ++i) {
                if (fabs(node.price - base_[i].price) < EPS) {
                    if (fabs(node.amount) < EPS) {
                        remove(i);
                    } else {
                        base_[i].amount = node.amount;
                    }
                    return;
                }
            }
            push(node);
        }

    private:
        std::vector<snapshot::data> base_;
        Comp comp_;
    };

    heap<std::less<>> asks{};
    heap<std::greater<>> bids{};

    explicit impl(const snapshot &init) {
        asks.build(init.asks);
        bids.build(init.bids);
    }

    void update(const snapshot &update) {
        for(const auto &node : update.asks) {
            asks.update(node);
        }
        for(const auto &node : update.bids){
            bids.update(node);
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
    auto best_ask = impl_->asks.top();
    auto best_bid = impl_->bids.top();
    return {best_ask.price, best_ask.amount, best_bid.price, best_bid.amount};
}