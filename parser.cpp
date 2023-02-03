#include <vector>
#include <string>

#include "parser.h"
#include "json.hpp"

namespace parser {
    std::vector<snapshot::data> parse_data_vector(const nlohmann::json::array_t &input){
        std::vector<snapshot::data> result;
        for(const auto& each: input) {
            result.emplace_back(each[0].get<double>(), each[1].get<double>());
        }
        return result;
    }

    snapshot parse_snapshot(const std::string &input) {
        snapshot result;
        auto pos = input.find('{');
        if (pos != std::string::npos) {
            nlohmann::json data = nlohmann::json::parse(input.substr(pos));
            result.asks = parse_data_vector(data["asks"]);
            result.bids = parse_data_vector(data["bids"]);
            result.timestamp_ms = data["event_time"];
        }
        return result;
    }

}// parser
