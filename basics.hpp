#ifndef __BASICS_HXX_INCLUDED
#define __BASICS_HXX_INCLUDED
#include <algorithm>
#include <vector>
#include <set>
#include <random>
#include <utility>
#include <map>
#include <csignal>
#include <chrono>
#include <cmath>
#include <cstring>
#include <cinttypes>
#include <type_traits>

static size_t compareCount = 0;

class myElement {
    int value;
public:
    myElement(int v = 0) : value(v) {}
    bool operator<(const myElement& other) const noexcept {
        compareCount++;
        return value < other.value;
    }
    bool operator<=(const myElement& other) const noexcept {
        compareCount++;
        return value <= other.value;
    }
    bool operator>(const myElement& other) const noexcept {
        compareCount++;
        return value > other.value;
    }
    bool operator>=(const myElement& other) const noexcept {
        compareCount++;
        return value >= other.value;
    }
    bool operator==(const myElement& other) const noexcept {
        compareCount++;
        return value == other.value;
    }
};

using myContainer = std::vector<myElement>;
#endif