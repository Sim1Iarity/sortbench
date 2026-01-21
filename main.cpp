#include "gens.hpp"
#include "sorts.hpp"

std::vector<sortAlgo*> flagToAlgo(unsigned int flag) {
    std::vector<sortAlgo*> result;
    for (int i = 0; i < sizeof(sortalgos)/sizeof(sortAlgo*); i++) {
        if (flag & (1u << i)) result.push_back(sortalgos[i]);
    }
    return result;
}

std::vector<std::pair<genIndices*, std::string> > flagToGen(unsigned int flag) {
    std::vector<std::pair<genIndices*, std::string> > result;
    for (int i = 0; i < sizeof(generators)/sizeof(generators[0]); i++) {
        if (flag & (1u << i)) result.push_back(generators[i]);
    }
    return result;
}

void stackOverflowHandler(int) {
    fprintf(stderr, "\033[1;31mStack Overflown!\033[0m\n");
    _exit(1);
}

void bench_worker(unsigned int flag, unsigned int flagGen, int n) {
    std::vector<sortAlgo*> algos = flagToAlgo(flag);
    std::vector<std::pair<genIndices*, std::string> > gens = flagToGen(flagGen);
    for (auto [gen, genstr] : gens) {
        std::vector<myElement> indices(n);
        gen->generate(indices, n);
        printf("| %d(%s) |", n, genstr.c_str());
        for (auto algo : algos) {
            myContainer indices_sorted;
            try {
                indices_sorted = indices;
            } catch (std::bad_alloc& e) {
                fprintf(stderr, "*** Fatal error: Not enough memory! bailing...\n");
                exit(1);
            }
            compareCount = 0;
            std::chrono::steady_clock::time_point t = std::chrono::steady_clock::now();
            algo->sort(indices_sorted);
            std::chrono::steady_clock::duration d = std::chrono::steady_clock::now() - t;
            printf(" $%.3lf\\text{ms}(%llu)$ |", d.count() / 1000000.0, compareCount);
        }
        putchar('\n');
    }
}

void bench(unsigned int flag, unsigned int flagGen, std::vector<int> nArr) {
    printf("| data size |");
    int count = 1;
    for (int i = 0; i < sizeof(sortalgos)/sizeof(sortAlgo); i++) {
        if (flag & (1u << i)) {
            count++;
            printf(" %s |", algoDescription[i].c_str());
        }
    }
    putchar('\n');
    for (int i = 0; i < count; i++) printf("|:-:");
    puts("|");
    for (int n : nArr) bench_worker(flag, flagGen, n);
}

int main() {
    signal(SIGSEGV, stackOverflowHandler);
    std::vector<int> nArr;
    int nelement;
    unsigned int flag, flagGen;
    scanf("%d%u%u", &nelement, &flag, &flagGen);
    nArr.resize(nelement);
    for (int i = 0; i < nelement; i++) scanf("%d", &nArr[i]);
    bench(flag, flagGen, nArr);
    return 0;
}