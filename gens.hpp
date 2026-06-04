#include "basics.hpp"

class genIndices {
public:
    virtual void generate(myElement*, int) = 0;
    virtual ~genIndices() {}
};

class RandomNumberGenerator {
    unsigned int state;
public:
    typedef unsigned int result_type;
    constexpr RandomNumberGenerator(const RandomNumberGenerator&) = default;
    constexpr RandomNumberGenerator(RandomNumberGenerator&&) = default;
    constexpr RandomNumberGenerator(unsigned int s) : state(s ? s : 1812433253U) {}
    constexpr RandomNumberGenerator& operator=(const RandomNumberGenerator&) = default;
    constexpr RandomNumberGenerator& operator=(RandomNumberGenerator&&) = default;
    constexpr void seed(unsigned int s) { state = s ? s : 1812433253U; }
    static constexpr unsigned int min() { return 1u; }
    static constexpr unsigned int max() { return ~0u; }
    constexpr unsigned int operator()() {
        state ^= state << 13;
        state ^= state >> 7;
        state ^= state << 17;
        return state;
    }
};

class RandomIndices : public genIndices {
    bool isPowerOf2(int n) { return (n & (-n)) == n; }
public:
    void generate(myElement* indices, int n) override {
        RandomNumberGenerator rng(std::random_device{}());
        for (int i = 0; i < n; i++) {
            indices[i] = i;
        }
        if (isPowerOf2(n)) {
            int bits = 32 - std::log2(n);
            for (int i = 0; i < n; i++) {
                int idx = rng() >> bits;
                if (i != idx) std::swap(indices[i], indices[idx]);
            }
        }
        else {
            const __uint128_t invn = ((__uint128_t)1 << 96) / n;
            for (int i = 0; i < n; i++) {
                int idx = rng();
                idx -= n * ((invn * idx) >> 96);
                if (idx >= n) idx -= n;
                if (i != idx) std::swap(indices[i], indices[idx]);
            }
        }
    }
};

class AscendingIndices : public genIndices {
public:
    void generate(myElement* indices, int n) override {
        for (int i = 0; i < n; i++) {
            indices[i] = i;
        }
    }
};

class DescendingIndices : public genIndices {
public:
    void generate(myElement* indices, int n) override {
        for (int i = 0; i < n; i++) {
            indices[i] = n - i - 1;
        }
    }
};

class PipeOrganIndices : public genIndices {
public:
    void generate(myElement* indices, int n) override {
        for (int i = 0; i < (n >> 1); i++) {
            indices[i] = 2 * i + 1;
        }
        for (int i = n >> 1; i < n; i++) {
            indices[i] = 2 * (n - i - 1);
        }
    }
};

class FinalMergeIndices : public genIndices {
public:
    void generate(myElement* indices, int n) override {
        for (int i = 0; i < n; i++) {
            indices[i] = i;
        }
        std::swap_ranges(indices, indices + (n / 2), indices + (n / 2));
    }
};

class FinalMergeReversedIndices : public genIndices {
public:
    void generate(myElement* indices, int n) override {
        for (int i = 0; i < n / 2; i++) {
            indices[i] = n - 2 * i - 1;
        }
        for (int i = n / 2; i < n; i++) {
            indices[i] = 2 * (n - i - 1);
        }
    }
};

class HeapifiedIndices : public genIndices {
public:
    void generate(myElement* indices, int n) override {
        for (int i = 0; i < n; i++) {
            indices[i] = i;
        }
        std::make_heap(indices, indices + n);
    }
};

class BSTreeIndices : public genIndices {
public:
    void generate(myElement* indices, int n) override {
        auto make_tree = [&](auto&& self, int left, int right, int level) -> void {
            if (left == right) {
                indices[left] = left * level - 1;
                return;
            }
            int mid = left + (right - left) / 2;
            self(self, left, mid, level * 2);
            for (int i = mid; i < right; i++) {
                indices[i] = 2 * level * (i - mid) + level - 1;
            }
        };
        make_tree(make_tree, 0, n, 1);
    }
};

class SawtoothIndices : public genIndices {
public:
    void generate(myElement* indices, int n) override {
        int new_n = (n / 4) * 4;
        for (int i = 0; i < new_n / 4; i++) {
            indices[i] = 4 * i;
            indices[i + new_n / 4] = 4 * i + 1;
            indices[i + new_n / 2] = 4 * i + 2;
            indices[i + 3 * new_n / 4] = 4 * i + 3;
        }
        for (int i = new_n; i < n; i++) indices[i] = i;
    }
};

class ScrambledTailIndices : public genIndices {
public:
    void generate(myElement* indices, int n) override {
        int curpos = 0, curnum = 1;
        while (true) {
            if (curnum % 8 == 0) curnum++;
            if (curnum >= n) break;
            indices[curpos++] = curnum;
            curnum++;
        }
        for (int i = curpos; i < n; i++) {
            indices[i] = (i - curpos) * 8;
        }
        std::shuffle(indices + curpos, indices + n, RandomNumberGenerator(std::random_device{}()));
    }
};

class ScrambledHeadIndices : public genIndices {
public:
    void generate(myElement* indices, int n) override {
        int curpos = 0, curnum = 1;
        while (true) {
            if (curnum % 8 == 0) curnum++;
            if (curnum >= n) break;
            indices[curpos++] = curnum;
            curnum++;
        }
        for (int i = curpos; i < n; i++) {
            indices[i] = (i - curpos) * 8;
        }
        std::shuffle(indices + curpos, indices + n, std::mt19937(std::random_device{}()));
        std::rotate(indices, indices + curpos, indices + n);
    }
};

class HalfReverseIndices : public genIndices {
public:
    void generate(myElement* indices, int n) override {
        for (int i = 0; i < n; i++) {
            indices[i] = i;
        }
        size_t i = 4, j = n - 1;
        while (i < j) {
            std::swap(indices[i], indices[j]);
            i += 2;
            j -= 2;
        }
        std::swap(indices[0], indices[n - 1]);
    }
};

// A quicksort-adversarial index generator
// https://www.cs.dartmouth.edu/~doug/mdmspe.pdf
class AdversaryIndices : public genIndices {
public:
    void generate(myElement* indices, int n) override {
        int num = 0;
        std::map<int, std::vector<int> > positions;
        auto lowbit = [=](int x) -> int {
            return x & (-x);
        };
        for (int i = 0; i < n / 2; i++) {
            int lb = lowbit(i + 1);
            positions[lb].push_back(i);
        }
        for (auto& p : positions) {
            for (int pos : p.second) {
                indices[pos] = num;
                num += 2;
            }
        }
        for (int i = n / 2; i < n; i++) indices[i] = (i - n / 2) * 2 + 1;
    }
};

const std::pair<genIndices*, std::string> generators[] =
{
    {new RandomIndices(), "random"},
    {new AscendingIndices(), "ascending"},
    {new DescendingIndices(), "descending"},
    {new PipeOrganIndices(), "pipe organ"},
    {new FinalMergeIndices(), "final merge"},
    {new FinalMergeReversedIndices(), "final merge reversed"},
    {new HeapifiedIndices(), "heapified"},
    {new BSTreeIndices(), "binary search tree"},
    {new SawtoothIndices(), "sawtooth"},
    {new ScrambledHeadIndices(), "scrambled head"},
    {new ScrambledTailIndices(), "scrambled tail"},
    {new HalfReverseIndices(), "half-reversed"},
    {new AdversaryIndices(), "adversary"}
};

const std::string genNames[] =
{
    "rand",
    "asc",
    "desc",
    "pipeorgan",
    "merge",
    "mergerev",
    "heap",
    "bst",
    "saw",
    "randhead",
    "randtail",
    "halfrev",
    "adversary"
};