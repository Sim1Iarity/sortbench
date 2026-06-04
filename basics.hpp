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
static size_t accessCount = 0;

class myElement {
    int value;
public:
    myElement(int v = 0) : value(v) {}
    myElement(const myElement& rhs) : value(rhs.value) { ++accessCount; }
    myElement(myElement&& rhs) : value(rhs.value) { ++accessCount; }
    myElement& operator=(const myElement& rhs) {
        value = rhs.value;
        ++accessCount;
        return *this;
    }
    myElement& operator=(myElement&& rhs) {
        value = rhs.value;
        ++accessCount;
        return *this;
    }
    inline bool operator<(const myElement& other) const noexcept {
        compareCount++;
        return value < other.value;
    }
    inline bool operator<=(const myElement& other) const noexcept {
        compareCount++;
        return value <= other.value;
    }
    inline bool operator>(const myElement& other) const noexcept {
        compareCount++;
        return value > other.value;
    }
    inline bool operator>=(const myElement& other) const noexcept {
        compareCount++;
        return value >= other.value;
    }
    inline bool operator==(const myElement& other) const noexcept {
        compareCount++;
        return value == other.value;
    }
    inline bool operator!=(const myElement& other) const noexcept {
        compareCount++;
        return value != other.value;
    }
};

class myContainer;

class myIterator {
    friend class myContainer;
protected:
    std::vector<myElement>::iterator m_it;
public:
    typedef myElement value_type;
    typedef ptrdiff_t difference_type;
    typedef std::random_access_iterator_tag iterator_category;
    typedef myElement* pointer;
    typedef myElement& reference;
    inline constexpr myIterator() = default;
    inline constexpr myIterator(const myIterator&) = default;
    inline constexpr myIterator(myIterator&&) = default;
    inline constexpr myIterator& operator=(const myIterator&) = default;
    inline constexpr myIterator& operator=(myIterator&&) = default;
    inline constexpr myIterator(const std::vector<myElement>::iterator& rhs) : m_it(rhs) {}
    inline myIterator(pointer rhs) : m_it(rhs) {}
    inline constexpr myIterator& operator=(const std::vector<myElement>::iterator& rhs) {
        m_it = rhs;
        return *this;
    }
    inline bool operator<(const myIterator& rhs) const { return m_it < rhs.m_it; }
    inline bool operator>(const myIterator& rhs) const { return m_it > rhs.m_it; }
    inline bool operator<=(const myIterator& rhs) const { return m_it <= rhs.m_it; }
    inline bool operator>=(const myIterator& rhs) const { return m_it >= rhs.m_it; }
    inline bool operator!=(const myIterator& rhs) const { return m_it != rhs.m_it; }
    inline bool operator==(const myIterator& rhs) const { return m_it == rhs.m_it; }
    inline myElement& operator*() {
        ++accessCount;
        return *m_it;
    }
    inline myIterator& operator++() { ++m_it; return *this; }
    inline myIterator operator++(int) { m_it++; return *this; }
    inline myIterator& operator--() { --m_it; return *this; }
    inline myIterator operator--(int) { m_it--; return *this; }
    inline ptrdiff_t operator-(const myIterator& rhs) const { return m_it - rhs.m_it; }
    inline myIterator& operator+=(ptrdiff_t a) { m_it += a; return *this; }
    inline myIterator& operator-=(ptrdiff_t a) { m_it -= a; return *this; }
    inline myIterator operator+(ptrdiff_t a) const { return myIterator(*this) += a; }
    inline myIterator operator-(ptrdiff_t a) const { return myIterator(*this) -= a; }
};

class myContainer {
    std::vector<myElement> m_array;
    friend class myIterator;
public:
    typedef myElement value_type;
    typedef myElement* pointer;
    typedef myIterator iterator;
    typedef const myIterator const_iterator;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef std::vector<myElement>::allocator_type allocator_type;
    inline myContainer() = default;
    inline myContainer(const myContainer& rhs) : m_array(rhs.m_array) { accessCount += rhs.m_array.size(); }
    inline myContainer(myContainer&& rhs) : m_array(rhs.m_array) { accessCount += rhs.m_array.size(); }
    inline myContainer(myIterator begin, myIterator end) : m_array(begin.m_it, end.m_it) { accessCount += end - begin; }
    inline myContainer& operator=(const myContainer& rhs) {
        m_array = rhs.m_array;
        accessCount += m_array.size();
        return *this;
    }
    inline myContainer& operator=(myContainer&& rhs) {
        m_array = rhs.m_array;
        accessCount += m_array.size();
        return *this;
    }
    inline size_t size() const { return m_array.size(); }
    inline myElement& operator[](size_t idx) {
        ++accessCount;
        return m_array[idx];
    }
    inline myIterator begin() { return myIterator(m_array.begin()); }
    inline myIterator end() { return myIterator(m_array.end()); }
    inline void resize(size_t n) { m_array.resize(n); }
};

//using myContainer = std::vector<myElement>;
#endif