#pragma once

#include <vector>
#include <string>

#include "json.hpp"

struct snapshot {
    struct data{
        double price;
        double amount;
        data() = default;
        data(double price, double amount) :
                price(price),
                amount(amount)
        {
        }
        bool operator<(const data& other) const {
            return price < other.price;
        }
        bool operator>(const data& other) const {
            return price > other.price;
        }
    };
    std::vector<data> asks;
    std::vector<data> bids;
    int64_t timestamp_ms;
};


namespace parser {
    std::vector<snapshot::data> parse_data_vector(const nlohmann::json::array_t &input);
    snapshot parse_snapshot(const std::string &input);

}// parser

