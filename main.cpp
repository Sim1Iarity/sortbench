#include "gens.hpp"
#include "sorts.hpp"

std::vector<sortAlgo*> flagToAlgo(unsigned int flag) {
    std::vector<sortAlgo*> result;
    for (unsigned i = 0; i < sizeof(sortalgos)/sizeof(sortAlgo*); i++) {
        if (flag & (1u << i)) result.push_back(sortalgos[i]);
    }
    return result;
}

std::vector<std::pair<genIndices*, std::string> > flagToGen(unsigned int flag) {
    std::vector<std::pair<genIndices*, std::string> > result;
    for (unsigned i = 0; i < sizeof(generators)/sizeof(generators[0]); i++) {
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
        myElement* indices = (myElement*)malloc(n * sizeof(myElement));
        gen->generate(indices, n);
        printf("| %d(%s) |", n, genstr.c_str());
        fflush(stdout);
        for (auto algo : algos) {
            myContainer indices_sorted;
            try {
                indices_sorted.resize(n);
                memcpy(indices_sorted.data(), indices, n * sizeof(myElement));
            } catch (std::bad_alloc& e) {
                fprintf(stderr, "*** Fatal error: Not enough memory! bailing...\n");
                exit(1);
            }
            compareCount = 0;
            std::chrono::steady_clock::time_point t = std::chrono::steady_clock::now();
            algo->sort(indices_sorted);
            std::chrono::steady_clock::duration d = std::chrono::steady_clock::now() - t;
            printf(" $%.6lf\\text{ms}(%" PRIu64 ")$ |", d.count() / 1000000.0, compareCount);
            fflush(stdout);
        }
        putchar('\n');
        free(indices);
    }
}

void bench(unsigned int flag, unsigned int flagGen, std::vector<int> nArr) {
    printf("| data size |");
    int count = 1;
    for (unsigned i = 0; i < sizeof(sortalgos)/sizeof(sortAlgo); i++) {
        if (flag & (1u << i)) {
            count++;
            printf(" %s |", algoDescription[i][1].c_str());
        }
    }
    putchar('\n');
    for (int i = 0; i < count; i++) printf("|:-:");
    puts("|");
    for (int n : nArr) bench_worker(flag, flagGen, n);
}

void print_usage(char* argv[]) {
    printf("Sortbench - benchmarking sorting algorithms\n");
    printf("Usage: %s [-a algo]... [-g gen]... [-o outputfile] @filename\n", argv[0]);
    printf("   or: %s [-a algo]... [-g gen]... [-o outputfile] ...\n", argv[0]);
    printf("Option [-a algo]... can be replaced with [-a algo1,algo2,...] or [-a=algo1,algo2,...]. Similar for generators.\n");
    printf("-a can be replaced by --algo; -g can be replaced by --gen\n");
    printf("Valid options for algo: \n");
    for (int i = 0; i < 27; i++) {
        printf("\t%s: %s\n", algoDescription[i][0].c_str(), algoDescription[i][1].c_str());
    }
    printf("Valid options for gen: \n");
    for (int i = 0; i < 13; i++) {
        printf("\t%s: %s\n", genNames[i].c_str(), generators[i].second.c_str());
    }
}

int find_index(bool is_gen, std::string opt) {
    for (char& c : opt) c = tolower(c);
    if (is_gen) {
        int ret = std::find(genNames, genNames + 13, opt) - genNames;
        if (ret == 13) return -1;
        return ret;
    }
    for (int i = 0; i < 20; i++) if (algoDescription[i][0] == opt) return i;
    return -1;
}

void parse_args(std::vector<int>& nArr, unsigned int& flag, unsigned int& flagGen, int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            if (argv[i][1] == 'a') {
                if (strlen(argv[i]) == 2) {
                    i++;
                    if (i == argc) {
                        printf("Error: algorithms not specified\n");
                        printf("See --help for a full list of algorithms\n");
                        exit(1);
                    }
                    char *token = strtok(argv[i], ",");
                    while (token != NULL) {
                        int num = find_index(false, token);
                        if (num == -1) {
                            printf("Error: unknown algorithm %s\n", token);
                            printf("See --help for a full list of algorithms\n");
                            exit(1);
                        }
                        flag |= 1u << num;
                        token = strtok(NULL, ",");
                    }
                }
                else if (argv[i][2] == '=') {
                    if (strlen(argv[i]) == 3) {
                        printf("Error: algorithms not specified\n");
                        printf("See --help for a full list of algorithms\n");
                        exit(1);
                    }
                    char *token = strtok(argv[i] + 3, ",");
                    while (token != NULL) {
                        int num = find_index(false, token);
                        if (num == -1) {
                            printf("Error: unknown algorithm %s\n", token);
                            printf("See --help for a full list of algorithms\n");
                            exit(1);
                        }
                        flag |= 1u << num;
                        token = strtok(NULL, ",");
                    }
                }
                else {
                    printf("Error: unknown option %s\n", argv[i]);
                    printf("See --help for more information\n");
                    exit(1);
                }
            }
            else if (argv[i][1] == 'g') {
                if (strlen(argv[i]) == 2) {
                    i++;
                    if (i == argc) {
                        printf("Error: generators not specified\n");
                        printf("See --help for a full list of generators\n");
                        exit(1);
                    }
                    char *token = strtok(argv[i], ",");
                    while (token != NULL) {
                        int num = find_index(true, token);
                        if (num == -1) {
                            printf("Error: unknown generator %s\n", token);
                            printf("See --help for a full list of generators\n");
                            exit(1);
                        }
                        flagGen |= 1u << num;
                        token = strtok(NULL, ",");
                    }
                }
                else if (argv[i][2] == '=') {
                    if (strlen(argv[i]) == 3) {
                        printf("Error: generators not specified\n");
                        printf("See --help for a full list of generators\n");
                        exit(1);
                    }
                    char *token = strtok(argv[i] + 3, ",");
                    while (token != NULL) {
                        int num = find_index(true, token);
                        if (num == -1) {
                            printf("Error: unknown generator %s\n", token);
                            printf("See --help for a full list of generators\n");
                            exit(1);
                        }
                        flagGen |= 1u << num;
                        token = strtok(NULL, ",");
                    }
                }
                else {
                    printf("Error: unknown option %s\n", argv[i]);
                    printf("See --help for more information\n");
                    exit(1);
                }
            }
            else if (argv[i][1] == 'h') {
                if (strlen(argv[i]) != 2) {
                    printf("Error: unknown option %s\n", argv[i]);
                    printf("See --help for more information\n");
                    exit(1);
                }
                print_usage(argv);
                exit(0);
            }
            else if (argv[i][1] == 'o') {
                if (strlen(argv[i]) == 2) {
                    i++;
                    if (i == argc) {
                        printf("Error: output filename not specified\n");
                        exit(1);
                    }
                    if (freopen(argv[i], "at", stdout) == NULL) {
                        std::string err_msg = std::string("Error: cannot open file ") + argv[i];
                        perror(err_msg.c_str());
                        exit(errno);
                    }
                }
                else if (argv[i][2] == '=') {
                    if (strlen(argv[i]) == 3) {
                        printf("Error: output file not specified\n");
                        printf("See --help for more information\n");
                        exit(1);
                    }
                    if (freopen(argv[i] + 3, "at", stdout) == NULL) {
                        std::string err_msg = std::string("Error: cannot open file ") + (argv[i] + 3);
                        perror(err_msg.c_str());
                        exit(errno);
                    }
                }
            }
            else if (argv[i][1] == '-') {
                if (strlen(argv[i]) <= 4) {
                    printf("Error: unknown option %s\n", argv[i]);
                    printf("See --help for more information\n");
                    exit(1);
                }
                if (std::equal(argv[i], argv[i] + 5, "--gen")) {
                    if (strlen(argv[i]) == 5) {
                        i++;
                        if (i == argc) {
                            printf("Error: generators not specified\n");
                            printf("See --help for a full list of generators\n");
                            exit(1);
                        }
                        char *token = strtok(argv[i], ",");
                        while (token != NULL) {
                            int num = find_index(true, token);
                            if (num == -1) {
                                printf("Error: unknown generator %s\n", token);
                                printf("See --help for a full list of generators\n");
                                exit(1);
                            }
                            flagGen |= 1u << num;
                            token = strtok(NULL, ",");
                        }
                    }
                    else if (argv[i][5] == '=') {
                        if (strlen(argv[i]) == 6) {
                            printf("Error: generators not specified\n");
                            printf("See --help for a full list of generators\n");
                            exit(1);
                        }
                        char *token = strtok(argv[i] + 6, ",");
                        while (token != NULL) {
                            int num = find_index(true, token);
                            if (num == -1) {
                                printf("Error: unknown generator %s\n", token);
                                printf("See --help for a full list of generators\n");
                                exit(1);
                            }
                            flagGen |= 1u << num;
                            token = strtok(NULL, ",");
                        }
                    }
                    else {
                        printf("Error: unknown option %s\n", argv[i]);
                        printf("See --help for more information\n");
                        exit(1);
                    }
                }
                else if (std::equal(argv[i], argv[i] + 5, "--out")) {
                    if (strlen(argv[i]) == 5) {
                        i++;
                        if (i == argc) {
                            printf("Error: output filename not specified\n");
                            exit(1);
                        }
                        if (freopen(argv[i], "at", stdout) == NULL) {
                            std::string err_msg = std::string("Error: cannot open file ") + argv[i];
                            perror(err_msg.c_str());
                            exit(errno);
                        }
                    }
                    else if (argv[i][5] == '=') {
                        if (strlen(argv[i]) == 6) {
                            printf("Error: output file not specified\n");
                            printf("See --help for more information\n");
                            exit(1);
                        }
                        if (freopen(argv[i] + 6, "at", stdout) == NULL) {
                            std::string err_msg = std::string("Error: cannot open file ") + (argv[i] + 6);
                            perror(err_msg.c_str());
                            exit(errno);
                        }
                    }
                }
                else if (strlen(argv[i]) == 5) {
                    printf("Error: unknown option %s\n", argv[i]);
                    printf("See --help for more information\n");
                    exit(1);
                }
                else if (std::equal(argv[i], argv[i] + 6, "--algo")) {
                    if (strlen(argv[i]) == 6) {
                        i++;
                        if (i == argc) {
                            printf("Error: algorithms not specified\n");
                            printf("See --help for a full list of algorithms\n");
                            exit(1);
                        }
                        char *token = strtok(argv[i], ",");
                        while (token != NULL) {
                            int num = find_index(false, token);
                            if (num == -1) {
                                printf("Error: unknown algorithm %s\n", token);
                                printf("See --help for a full list of algorithms\n");
                                exit(1);
                            }
                            flag |= 1u << num;
                            token = strtok(NULL, ",");
                        }
                    }
                    else if (argv[i][6] == '=') {
                        if (strlen(argv[i]) == 7) {
                            printf("Error: algorithms not specified\n");
                            printf("See --help for a full list of algorithms\n");
                            exit(1);
                        }
                        char *token = strtok(argv[i] + 7, ",");
                        while (token != NULL) {
                            int num = find_index(false, token);
                            if (num == -1) {
                                printf("Error: unknown algorithm %s\n", token);
                                printf("See --help for a full list of algorithms\n");
                                exit(1);
                            }
                            flag |= 1u << num;
                            token = strtok(NULL, ",");
                        }
                    }
                    else {
                        printf("Error: unknown option %s\n", argv[i]);
                        printf("See --help for more information\n");
                        exit(1);
                    }
                }
                else if (std::equal(argv[i], argv[i] + 6, "--help")) {
                    if (strlen(argv[i]) != 6) {
                        printf("Error: unknown option %s\n", argv[i]);
                        printf("See --help for more information\n");
                        exit(1);
                    }
                    print_usage(argv);
                    exit(0);
                }
                else {
                    printf("Error: unknown option %s\n", argv[i]);
                    printf("See --help for more information\n");
                    exit(1);
                }
            }
            else {
                printf("Error: unknown option %s\n", argv[i]);
                printf("See --help for more information\n");
                exit(1);
            }
        }
        else if (argv[i][0] == '@') {
            if (strlen(argv[i]) == 1) {
                printf("Error: unused option %s\n", argv[i]);
                printf("See --help for more information\n");
                exit(1);
            }
            FILE* inputfile = fopen(argv[i] + 1, "rt");
            if (inputfile == NULL) {
                std::string err_msg = std::string("Error: cannot open file ") + (argv[i] + 1);
                perror(err_msg.c_str());
                exit(errno);
            }
            int num = 0;
            while (fscanf(inputfile, "%d", &num) == 1) nArr.push_back(num);
            fclose(inputfile);
        }
        else if (argv[i][0] >= '1' && argv[i][0] <= '9') {
            int num = 0;
            if (sscanf(argv[i], "%d", &num) != 1) {
                printf("Error: unused option %s\n", argv[i]);
                printf("See --help for more information\n");
                exit(1);
            }
            nArr.push_back(num);
        }
        else {
            printf("Error: unused option %s\n", argv[i]);
            printf("See --help for more information\n");
            exit(1);
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        print_usage(argv);
        return 0;
    }
    signal(SIGSEGV, stackOverflowHandler);
    std::vector<int> nArr;
    unsigned int flag = 0, flagGen = 0;
    parse_args(nArr, flag, flagGen, argc, argv);
    if (flag == 0) {
        printf("Error: no algorithms to benchmark\n");
        exit(1);
    }
    if (flagGen == 0 || nArr.empty()) {
        printf("Error: no arrays to benchmark\n");
        exit(1);
    }
    bench(flag, flagGen, nArr);
    return 0;
}