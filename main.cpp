#include <iostream>
#include <fstream>
#include <sstream>
#include <tuple>
#include <chrono>

#include "parser.h"
#include "impl/order_book.h"

std::string prettify(int64_t ts, std::tuple<double, double, double, double> tops) {
    //{event_time}, {best bid price}, {best bid amount}, {best ask price}, {best ask amount}
    char res[256];
    sprintf(res, "{%ld}, {%.2f}, {%f}, {%.2f}, {%f}",
            ts, get<0>(tops), get<1>(tops), get<2>(tops), get<3>(tops));
    return res;
}

struct chrono_timer {
    uint64_t total_time = 0;
    std::chrono::time_point<std::chrono::steady_clock> prev_time;
    void start() {
        prev_time = std::chrono::steady_clock::now();
    }
    void finish() {
        total_time += std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::steady_clock::now() - prev_time).count();
    }
};

int main(int argc, char** argv) {
    std::ifstream f(argv[1]);
    std::ofstream out(argv[2]);
    std::string input;
    std::getline(f, input);
    chrono_timer my_timer;
    my_timer.start();
    auto book = order_book(parser::parse_snapshot(input));
    my_timer.finish();
    while(std::getline(f, input)) {
        auto snap = parser::parse_snapshot(input);
        my_timer.start();
        book.update(snap);
        auto tops = book.get_tops();
        my_timer.finish();
        out << prettify(snap.timestamp_ms, tops) << '\n';
    }
    std::cout << "Total working time: " << my_timer.total_time << '\n';
    return 0;
}
