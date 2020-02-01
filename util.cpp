//
// Created by noah on 01-02-20.
//

#include <string>
#include <iostream>
#include <sstream>

std::string uint128_str(__uint128_t v) {
    uint64_t high = v>>64;
    uint64_t low = v&(0xFFFFFFFFFFFFFFFF);
    std::ostringstream oss;
    oss << high << " " << low;
    return oss.str();
}

__uint128_t str_uint128(std::string v) {
    std::stringstream ss;
    ss << v;
    __uint128_t res = 0;
    uint64_t high, low;
    ss >> high >> low;
    res = high;
    return (res<<64) | low;
}

