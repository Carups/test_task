#pragma once

#include "../parser.h"
#include <memory>

class order_book {
public:
    explicit order_book(const snapshot &init);
    void update(const snapshot &update);
    std::tuple<double, double, double, double> get_tops();
    ~order_book();
private:
    struct impl;
    std::unique_ptr<impl> impl_;
    static constexpr double EPS = 1e-8;
};



