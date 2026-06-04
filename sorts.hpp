#include "basics.hpp"

class sortAlgo {
public:
    virtual void sort(myContainer&) = 0;
    virtual ~sortAlgo() {}
};

static const std::string algoDescription[][2] = 
{
    {"stlsort", "std::sort"},
    {"stlstable", "std::stable_sort"},
    {"tim", "Timsort"},
    {"pdq", "PDQ-sort"},
    {"insertion", "Insertion sort"},
    {"selection", "Selection sort"},
    {"bubble", "Bubble sort"},
    {"cocktail", "Cocktail sort"},
    {"gnome", "Gnome sort"},
    {"comb", "Comb sort"},
    {"merge", "Merge sort"},
    {"heap", "Heap sort"},
    {"smooth", "Smooth sort"},
    {"shell", "Shell sort"},
    {"bitonic", "Bitonic sort"},
    {"quicklrfirst", "Quick sort (LR, pivot=first)"},
    {"quicklrlast", "Quick sort (LR, pivot=last)"},
    {"quicklrmid", "Quick sort (LR, pivot=middle)"},
    {"quicklrmed3", "Quick sort (LR, pivot=median of 3)"},
    {"quicklrmed9", "Quick sort (LR, pivot=median of 9)"},
    {"quicklrrand", "Quick sort (LR, pivot=random)"},
    {"quickllfirst", "Quick sort (LL, pivot=first)"},
    {"quicklllast", "Quick sort (LL, pivot=last)"},
    {"quickllmid", "Quick sort (LL, pivot=middle)"},
    {"quickllmed3", "Quick sort (LL, pivot=median of 3)"},
    {"quickllmed9", "Quick sort (LL, pivot=median of 9)"},
    {"quickllrand", "Quick sort (LL, pivot=random)"}
};

namespace nlognalgos {
class firstPivot {
public:
    static int selectPivot(myContainer&, int low, int) {
        return low;
    }
};
class lastPivot {
public:
    static int selectPivot(myContainer&, int, int high) {
        return high - 1;
    }
};
class middlePivot {
public:
    static int selectPivot(myContainer&, int low, int high) {
        return low + (high - low) / 2;
    }
};
class medianOfThreePivot {
    static int median3(myContainer& arr, int a, int b, int c) {
        if (arr[a] > arr[b]) std::swap(arr[a], arr[b]);
        if (arr[a] > arr[c]) std::swap(arr[a], arr[c]);
        if (arr[b] > arr[c]) std::swap(arr[b], arr[c]);
        return b;
    }
public:
    static int selectPivot(myContainer& arr, int low, int high) {
        int mid = low + (high - low) / 2;
        return median3(arr, low, mid, high - 1);
    }
};
class medianOfNinePivot {
    static void sort3(myContainer& arr, int a, int b, int c) {
        if (arr[a] > arr[b]) std::swap(arr[a], arr[b]);
        if (arr[a] > arr[c]) std::swap(arr[a], arr[c]);
        if (arr[b] > arr[c]) std::swap(arr[b], arr[c]);
    }
public:
    static int selectPivot(myContainer& arr, int begin, int end) {
        int s2 = (end - begin) / 2;
        sort3(arr, begin, begin + s2, end - 1);
        sort3(arr, begin + 1, begin + (s2 - 1), end - 2);
        sort3(arr, begin + 2, begin + (s2 + 1), end - 3);
        sort3(arr, begin + (s2 - 1), begin + s2, begin + (s2 + 1));
        return begin + s2;
    }
};
class randomPivot {
public:
    static int selectPivot(myContainer&, int low, int high) {
        static RandomNumberGenerator rng(std::random_device{}());
        return low + rng() % (high - low - 1);
    }
};
}

namespace stlalgos {
class STLSort : public sortAlgo {
public:
    void sort(myContainer& indices) override {
        std::sort(indices.begin(), indices.end());
    }
};

class STLStableSort : public sortAlgo {
public:
    void sort(myContainer& indices) override {
        std::stable_sort(indices.begin(), indices.end());
    }
};
}

namespace hybridalgos {

class timSort : public sortAlgo {
    static void mergeBackwards(myContainer& arr, int l, int m, int r) {
        myContainer rightArr(arr.begin() + m, arr.begin() + r);
        int i = m - 1, j = r - m - 1, k = r - 1;
        while (i >= l && j >= 0) {
            if (rightArr[j] >= arr[i]) {
                arr[k--] = rightArr[j--];
            } else {
                arr[k--] = arr[i--];
            }
        }
        if (j != -1) std::copy_backward(rightArr.begin(), rightArr.begin() + j + 1, arr.begin() + k + 1);
    }

    static void merge(myContainer& arr, int l, int m, int r) {
        if (r - m < m - l) {
            mergeBackwards(arr, l, m, r);
            return;
        }
        myContainer leftArr(arr.begin() + l, arr.begin() + m);
        int i = 0, j = m, k = l;
        int leftSize = leftArr.size();
        while (i < leftSize && j < r) {
            if (leftArr[i] <= arr[j]) {
                arr[k++] = leftArr[i++];
            } else {
                arr[k++] = arr[j++];
            }
        }
        if (i != leftSize) std::copy(leftArr.begin() + i, leftArr.end(), arr.begin() + k);
    }
public:
    void sort(myContainer& indices) override {
        int n = indices.size();
        std::vector<std::pair<int, int>> runStack;
        int minRun = n / (1 << (int)std::log2(n / 32));
        auto detectRun = [&](int start) -> int {
            if (start + 1 >= n) return start;
            if (indices[start] <=  indices[start + 1]) {
                int end = start + 1;
                while (end + 1 < n && indices[end] <= indices[end + 1]) end++;
                return end;
            } else {
                int end = start + 1;
                while (end + 1 < n && indices[end] >= indices[end + 1]) end++;
                int i = start, j = end;
                while (i < j) {
                    std::swap(indices[i], indices[j]);
                    i++; j--;
                }
                return end;
            }
        };
        auto insertionSort = [&](int left, int right) {
            for (int i = left + 1; i <= right; i++) {
                myElement temp = indices[i];
                int j = i - 1;
                while (j >= left && indices[j] > temp) {
                    indices[j + 1] = indices[j];
                    j--;
                }
                indices[j + 1] = temp;
            }
        };
        for (int i = 0; i < n;) {
            int runEnd = detectRun(i);
            int runLength = runEnd - i + 1;

            if (runLength < minRun) {
                int forceEnd = std::min(i + minRun - 1, n - 1);
                insertionSort(i, forceEnd);
                runStack.push_back({i, forceEnd});
                i = forceEnd + 1;
            } else {
                runStack.push_back({i, runEnd});
                i = runEnd + 1;
            }
            while (true) {
                bool conditionSatisfied = true;
                int m = runStack.size();

                if (m >= 3) {
                    auto &A = runStack[m - 3];
                    auto &B = runStack[m - 2];
                    auto &C = runStack[m - 1];
                    if (A.second - A.first < (B.second - B.first + C.second - C.first)) {
                        conditionSatisfied = false;
                        if (A.second - A.first < C.second - C.first) {
                            merge(indices, A.first, B.first, B.second + 1);
                            runStack[m - 3] = {A.first, B.second};
                            runStack[m - 2] = C;
                            runStack.pop_back();
                        } else {
                            merge(indices, B.first, C.first, C.second + 1);
                            runStack[m - 3] = A;
                            runStack[m - 2] = {B.first, C.second};
                            runStack.pop_back();
                        }
                        continue;
                    }
                }
                m = runStack.size();
                if (m >= 2) {
                    auto &A = runStack[m - 2];
                    auto &B = runStack[m - 1];
                    if (A.second - A.first < (B.second - B.first)) {
                        conditionSatisfied = false;
                        merge(indices, A.first, B.first, B.second + 1);
                        runStack[m - 2] = {A.first, B.second};
                        runStack.pop_back();
                        continue;
                    }
                }

                if (conditionSatisfied) break;
            }
        }
        while (runStack.size() > 1) {
            int m = runStack.size();
            auto &A = runStack[m - 2];
            auto &B = runStack[m - 1];
            merge(indices, A.first, B.first, B.second + 1);
            runStack[m - 2] = {A.first, B.second};
            runStack.pop_back();
        }
    }
};

template <bool useDNF = false>
class pdqSort : public sortAlgo {
public:
    void sort(myContainer& arr) override {
        int n = arr.size();
        sortRecursive(arr, 0, n, floor(log2(n)));
    }
private:
    static void insertionSort(myContainer& A, int left, int right) {
        for (int i = left + 1; i <= right; ++i) {
            myElement key = A[i];
            int j = i - 1;
            while (j >= left && A[j] > key) {
                A[j + 1] = A[j];
                j--;
            }
            A[j + 1] = key;
        }
    }

    static void mergeBackwards(myContainer& arr, int l, int m, int r) {
        if (m == l || m == r) return;
        myContainer rightArr(arr.begin() + m, arr.begin() + r);
        int i = m - 1, j = r - m - 1, k = r - 1;
        while (i >= l && j >= 0) {
            if (rightArr[j] > arr[i]) {
                arr[k--] = rightArr[j--];
            } else {
                arr[k--] = arr[i--];
            }
        }
        while (j >= 0) arr[k--] = rightArr[j--];
    }

    static void merge(myContainer& arr, int l, int m, int r) {
        if (m == l || m == r) return;
        if (r - m < m - l) {
            mergeBackwards(arr, l, m, r);
            return;
        }
        myContainer leftArr(arr.begin() + l, arr.begin() + m);
        int i = 0, j = m, k = l;
        int leftSize = leftArr.size();
        while (i < leftSize && j < r) {
            if (leftArr[i] <= arr[j]) {
                arr[k++] = leftArr[i++];
            } else {
                arr[k++] = arr[j++];
            }
        }
        while (i < leftSize) arr[k++] = leftArr[i++];
    }
    static void partition3(int& lt, int& gt, int& swapCount, myContainer& arr, int low, int high, bool use_med9) {
        int p;
        if (__builtin_expect(use_med9, false)) {
            p = ::nlognalgos::medianOfNinePivot::selectPivot(arr, low, high);
        }
        else p = ::nlognalgos::medianOfThreePivot::selectPivot(arr, low, high);
        myElement pivot = arr[p];
        lt = low, gt = high - 1;
        int i = low;
        while (i <= gt) {
            if (arr[i] < pivot) {
                if (p == lt) p = i;
                else if (p == i) p = lt;
                std::swap(arr[lt], arr[i]);
                swapCount++;
                lt++; i++;
            }
            else if (arr[i] > pivot) {
                if (p == gt) p = i;
                else if (p == i) p = gt;
                std::swap(arr[i], arr[gt]);
                swapCount++;
                gt--;
            }
            else i++;
        }
        return;
    }
    static int partition2(int& swapCount, myContainer& arr, int low, int high, bool use_med9) {
        int p;
        if (__builtin_expect(use_med9, false)) {
            p = ::nlognalgos::medianOfNinePivot::selectPivot(arr, low, high);
        }
        else p = ::nlognalgos::medianOfThreePivot::selectPivot(arr, low, high);
        myElement pivot = arr[p];
        while (true) {
            while (arr[low] < pivot) low++;
            --high;
            while (arr[high] > pivot) high--;
            if (low >= high) return low;
            std::swap(arr[low], arr[high]);
            swapCount++;
            ++low;
        }
    }

    static int detectRun(myContainer& arr, int start, int n) {
        if (arr[start] <= arr[start + 1]) {
            int end = start + 1;
            while (end + 1 < n && arr[end] <= arr[end + 1]) end++;
            return end;
        } else {
            int end = start + 1;
            while (end + 1 < n && arr[end] >= arr[end + 1]) end++;
            int ret = end;
            while (start < end) {
                std::swap(arr[start], arr[end]);
                start++;
                end--;
            }
            return ret;
        }
    }

    static int detectRunBackwards(myContainer& arr, int end, int) {
        if (arr[end - 1] >= arr[end]) {
            int pos = end - 1;
            while (arr[pos - 1] >= arr[pos]) pos--;
            int ret = pos;
            while (pos < end) {
                std::swap(arr[pos], arr[end]);
                pos++;
                end--;
            }
            return ret;
        }
        else {
            int pos = end - 1;
            while (arr[pos - 1] <= arr[pos]) pos--;
            return pos;
        }
    }

    static void sortRecursive(myContainer& arr, int low, int high, int badAllowed, int flag = 4, bool use_dnf = false, bool use_med9 = false) {
        while (high - low > 16) {
            int threshold = 31 - __builtin_clz(high - low);
            if (flag && high - low > 128) {
                int runEnd = detectRun(arr, low, high);
                if (runEnd >= high - 1) return;
                if (runEnd <= low + threshold) flag--;
                if (flag) {
                    int runStart = detectRunBackwards(arr, high - 1, low);
                    if (runStart - runEnd <= 1) {
                        merge(arr, low, runStart, high);
                        return;
                    }
                    if (runEnd >= low + 16 * threshold && runStart <= high - 16 * threshold) {
                        sortRecursive(arr, runEnd + 1, runStart, badAllowed);
                        merge(arr, low, runEnd + 1, runStart);
                        merge(arr, low, runStart, high);
                        return;
                    }
                    else if (runEnd >= low + 16 * threshold) {
                        sortRecursive(arr, runEnd + 1, high, badAllowed);
                        merge(arr, low, runEnd + 1, high);
                        return;
                    }
                    else if (runStart <= high - 16 * threshold) {
                        sortRecursive(arr, low, runStart, badAllowed);
                        merge(arr, low, runStart, high);
                        return;
                    }
                    if (runStart >= high - threshold) flag--;
                }
            }
            int pi_left, pi_right, swapCount = 0;
            if constexpr (useDNF) {
                if (use_dnf) {
                    partition3(pi_left, pi_right, swapCount, arr, low, high, use_med9);
                    if (pi_right - pi_left <= threshold) use_dnf = false;
                }
                else {
                    int pi = partition2(swapCount, arr, low, high, use_med9);
                    pi_left = pi - 1;
                    pi_right = pi;
                    if (arr[low] == arr[pi] || arr[high - 1] == arr[pi]) use_dnf = true;
                }
            }
            else {
                int pi = partition2(swapCount, arr, low, high, use_med9);
                pi_left = pi - 1;
                pi_right = pi;
            }
            int l_size = pi_left - low + 1;
            int r_size = high - pi_right;
            bool highly_unbalanced = l_size < r_size / 8 || r_size < l_size / 8;
            swapCount = std::min<int>(swapCount, high - low - swapCount);
            if (__builtin_expect(swapCount <= 8 && l_size >= 16 && r_size >= 16, false)) {
                int insertCount = 0, insertLimit = 48;
                for (int i = low + 1; i < high; i++) {
                    myElement key = arr[i];
                    int j = i - 1;
                    while (j >= low && arr[j] > key) {
                        j--;
                        insertCount++;
                        if (insertCount > insertLimit) break;
                    }
                    if (insertCount > insertLimit) break;
                }
                if (insertCount > insertLimit) goto pdqSortLoop;
                insertionSort(arr, low, high - 1);
                return;
            }
pdqSortLoop:
            if (highly_unbalanced) {
                if (__builtin_expect(--badAllowed == 0, false)) {
                    fprintf(stderr, "PDQ-sort calling heap sort on range [%d, %d)\n", low, high);
                    std::make_heap(arr.begin() + low, arr.begin() + high);
                    std::sort_heap(arr.begin() + low, arr.begin() + high);
                    return;
                }
                if (l_size > 16) {
                    std::swap(arr[low], arr[low + l_size / 4]);
                    std::swap(arr[pi_left - 1], arr[pi_left - l_size / 4]);
                    if (l_size > 128) {
                        std::swap(arr[low + 1], arr[low + l_size / 4 + 1]);
                        std::swap(arr[low + 2], arr[low + l_size / 4 + 2]);
                        std::swap(arr[pi_left - 2], arr[pi_left - l_size / 4 - 1]);
                        std::swap(arr[pi_left - 3], arr[pi_left - l_size / 4 - 2]);
                    }
                }
                if (r_size > 16) {
                    std::swap(arr[pi_right + 1], arr[pi_right + r_size / 4 + 1]);
                    std::swap(arr[high - 1], arr[high - r_size / 4]);
                    if (r_size > 128) {
                        std::swap(arr[pi_right + 2], arr[pi_right + r_size / 4 + 2]);
                        std::swap(arr[pi_right + 3], arr[pi_right + r_size / 4 + 3]);
                        std::swap(arr[high - 2], arr[high - r_size / 4 - 1]);
                        std::swap(arr[high - 3], arr[high - r_size / 4 - 2]);
                    }
                }
            }
            use_med9 = highly_unbalanced;
            if (r_size > l_size) {
                sortRecursive(arr, low, pi_left + 1, badAllowed, flag, use_dnf, highly_unbalanced);
                low = pi_right;
            }
            else {
                sortRecursive(arr, pi_right, high, badAllowed, flag, use_dnf, highly_unbalanced);
                high = pi_left + 1;
            }
        }
        insertionSort(arr, low, high - 1);
    }
};
}

namespace nsquaredalgos {
class insertionSort : public sortAlgo {
public:
    void sort(myContainer& indices) override {
        int n = indices.size();
        for (int i = 1; i < n; i++) {
            myElement key = indices[i];
            int j = i - 1;
            while (j >= 0 && indices[j] > key) {
                indices[j + 1] = indices[j];
                j--;
            }
            indices[j + 1] = key;
        }
    }
};

class selectionSort : public sortAlgo {
public:
    void sort(myContainer& indices) override {
        int n = indices.size();
        for (int i = 0; i < n - 1; i++) {
            int minIdx = i;
            for (int j = i + 1; j < n; j++) {
                if (indices[j] < indices[minIdx]) {
                    minIdx = j;
                }
            }
            std::swap(indices[i], indices[minIdx]);
        }
    }
};

class bubbleSort : public sortAlgo {
public:
    void sort(myContainer& indices) override {
        int newN, n = indices.size();
        while (n > 1) {
            newN = 0;
            for (int j = 0; j < n - 1; ++j) {
                if (indices[j] > indices[j + 1]) {
                    std::swap(indices[j], indices[j + 1]);
                    newN = j + 1;
                }
            }
            n = newN;
        }
    }
};

class cocktailSort : public sortAlgo {
public:
    void sort(myContainer& indices) override {
        int n = indices.size();
        int start = 0;
        int end = n - 1;

        while (start < end) {
            int newRight = start;
            for (int i = start; i < end; i++) {
                if (indices[i] > indices[i + 1]) {
                    std::swap(indices[i], indices[i + 1]);
                    newRight = i + 1;
                }
            }
            end = newRight;

            int newLeft = end;
            for (int i = end; i > start; i--) {
                if (indices[i - 1] > indices[i]) {
                    std::swap(indices[i - 1], indices[i]);
                    newLeft = i;
                }
            }
            start = newLeft;
        }
    }
};

class gnomeSort : public sortAlgo {
public:
    void sort(myContainer& indices) override {
        int n = indices.size();
        int index = 0;

        while (index < n) {
            if (index == 0 || indices[index] >= indices[index - 1]) {
                index++;
            } else {
                std::swap(indices[index], indices[index - 1]);
                index--;
            }
        }
    }
};

class combSort : public sortAlgo {
public:
    void sort(myContainer& indices) override {
        int n = indices.size();
        int gap = n;
        const double shrink = 1.3;
        bool sorted = false;

        while (!sorted) {
            gap = int(gap / shrink);
            if (gap < 1) gap = 1;
            sorted = true;

            for (int i = gap; i < n; i++) {
                if (indices[i - gap] > indices[i]) {
                    std::swap(indices[i - gap], indices[i]);
                    sorted = false;
                }
            }
        }
    }
};
}

namespace nlognalgos {
class mergeSort : public sortAlgo {
public:
    void sort(myContainer& indices) override {
        mergeSortRecursive(indices, 0, indices.size());
    }
private:
    void mergeSortRecursive(myContainer& arr, int left, int right) {
        if (right - left <= 1) return;
        int mid = left + (right - left) / 2;
        mergeSortRecursive(arr, left, mid);
        mergeSortRecursive(arr, mid, right);
        merge(arr, left, mid, right);
    }
    static void merge(myContainer& arr, int l, int m, int r) {
        myContainer leftArr(arr.begin() + l, arr.begin() + m);
        int i = 0, j = m, k = l;
        int leftSize = leftArr.size();

        while (i < leftSize && j < r) {
            if (leftArr[i] <= arr[j]) {
                arr[k++] = leftArr[i++];
            } else {
                arr[k++] = arr[j++];
            }
        }
        while (i < leftSize) {
            arr[k++] = leftArr[i++];
        }
        while (j < r) {
            arr[k++] = arr[j++];
        }
    }
};

class heapSort : public sortAlgo {
public:
    void sort(myContainer& indices) override {
        std::make_heap(indices.begin(), indices.end());
        std::sort_heap(indices.begin(), indices.end());
    }
};

static const int smoothSortLP[] = {
    1, 1, 3, 5, 9, 15, 25, 41, 67, 109,
    177, 287, 465, 753, 1219, 1973, 3193, 5167, 8361, 13529, 21891,
    35421, 57313, 92735, 150049, 242785, 392835, 635621, 1028457,
    1664079, 2692537, 4356617, 7049155, 11405773, 18454929, 29860703,
    48315633, 78176337, 126491971, 204668309, 331160281, 535828591,
    866988873 // the next number is > 31 bits.
};
class smoothSort : public sortAlgo {
public:
    void sort(myContainer& indices) override {
        sortLoop(indices, 0, indices.size() - 1);
    }
private:
    static void sift(myContainer& A, int pshift, int head) {
        myElement val = A[head];
        while (pshift > 1) {
            int rt = head - 1;
            int lf = head - 1 - smoothSortLP[pshift - 2];

            if (val >= A[lf] && val >= A[rt])
                break;

            if (A[lf] >= A[rt]) {
                A[head] = A[lf];
                head = lf;
                pshift -= 1;
            }
            else {
                A[head] = A[rt];
                head = rt;
                pshift -= 2;
            }
        }
        A[head] = val;
    }

    static void trinkle(myContainer& A, int p, int pshift, int head, bool isTrusty){
        myElement val = A[head];
        while (p != 1) {
            int stepson = head - smoothSortLP[pshift];
            if (A[stepson] <= val)
                break;
            if (!isTrusty && pshift > 1) {
                int rt = head - 1;
                int lf = head - 1 - smoothSortLP[pshift - 2];
                if (A[rt] >= A[stepson] || A[lf] >= A[stepson])
                    break;
            }
            A[head] = A[stepson];
            head = stepson;
            int trail = __builtin_ctz(p & ~1);
            p >>= trail;
            pshift += trail;
            isTrusty = false;
        }

        if (!isTrusty) {
            A[head] = val;
            sift(A, pshift, head);
        }
    }

    void sortLoop(myContainer& A, int lo, int hi) {
        int head = lo;
        int p = 1;
        int pshift = 1;

        while (head < hi) {
            if ((p & 3) == 3) {
                sift(A, pshift, head);
                p >>= 2;
                pshift += 2;
            }
            else {
                if (smoothSortLP[pshift - 1] >= hi - head) {
                    trinkle(A, p, pshift, head, false);
                } else {
                    sift(A, pshift, head);
                }

                if (pshift == 1) {
                    p <<= 1;
                    pshift--;
                } else {
                    p <<= (pshift - 1);
                    pshift = 1;
                }
            }
            p |= 1;
            head++;
        }
        trinkle(A, p, pshift, head, false);
        while (pshift != 1 || p != 1) {
            if (pshift <= 1) {
                int trail = __builtin_ctz(p & ~1);
                p >>= trail;
                pshift += trail;
            }
            else {
                p <<= 2;
                p ^= 7;
                pshift -= 2;
                trinkle(A, p >> 1, pshift + 1, head - smoothSortLP[pshift] - 1, true);
                trinkle(A, p, pshift, head - 1, true);
            }
            head--;
        }
    }
};

class shellSort : public sortAlgo {
public:
    void sort(myContainer& arr) override {
        int n = arr.size();
        for (int gap = n / 2; gap > 0; gap /= 3) {
            for (int i = gap; i < n; i++) {
                myElement temp = arr[i];
                int j;
                for (j = i; j >= gap && arr[j - gap] > temp; j -= gap) {
                    arr[j] = arr[j - gap];
                }
                arr[j] = temp;
            }
        }
    }
};

class bitonicSort : public sortAlgo {
private:
    static void compare(myContainer& A, int i, int j, bool dir) {
        if (dir == (A[i] > A[j])) std::swap(A[i], A[j]);
    }

    static int greatestPowerOfTwoLessThan(int n) {
        int k = 1;
        while (k < n) k = k << 1;
        return k >> 1;
    }

    static void bitonicMerge(myContainer& A, int lo, int n, bool dir) {
        if (n > 1) {
            int m = greatestPowerOfTwoLessThan(n);
            for (int i = lo; i < lo + n - m; i++)
                compare(A, i, i+m, dir);
            bitonicMerge(A, lo, m, dir);
            bitonicMerge(A, lo + m, n - m, dir);
        }
    }

    static void sortLoop(myContainer& A, int lo, int n, bool dir) {
        if (n > 1) {
            int m = n / 2;
            sortLoop(A, lo, m, !dir);
            sortLoop(A, lo + m, n - m, dir);
            bitonicMerge(A, lo, n, dir);
        }
    }
public:
    void sort(myContainer& indices) override {
        sortLoop(indices, 0, indices.size(), true);
    }
};
template <typename pivotSelector>
class quickSortLR : public sortAlgo {
public:
    void sort(myContainer& arr) override {
        quickSortRecursive(arr, 0, arr.size() - 1);
    }
private:
    void quickSortRecursive(myContainer& arr, int low, int high) {
        while (low < high) {
            if (__builtin_expect(high - low == 1, false)) {
                if (arr[high] < arr[low]) std::swap(arr[low], arr[high]);
                return;
            }
            static pivotSelector ps;
            int p = ps.selectPivot(arr, low, high + 1);
            myElement pivot = arr[p];
            int i = low;
            int j = high;
            while (i <= j) {
                while (arr[i] < pivot) i++;
                while (arr[j] > pivot) j--;
                if (__builtin_expect(i <= j, true)) {
                    std::swap(arr[i], arr[j]);
                    if      (p == i) p = j;
                    else if (p == j) p = i;
                    i++;
                    j--;
                }
            }
            if (j - low < high - i) {
                if (low < j) quickSortRecursive(arr, low, j);
                low = i;
            }
            else {
                if (i < high) quickSortRecursive(arr, i, high);
                high = j;
            }
        }
    }
};

template <typename pivotSelector>
class quicksortLL : public sortAlgo {
public:
    void sort(myContainer& arr) override {
        quickSortRecursive(arr, 0, arr.size() - 1);
    }
private:
    void quickSortRecursive(myContainer& arr, int low, int high) {
        while (low < high) {
            if (__builtin_expect(high - low == 1, false)) {
                if (arr[high] < arr[low]) std::swap(arr[low], arr[high]);
                return;
            }
            static pivotSelector ps;
            int p = ps.selectPivot(arr, low, high);
            myElement pivot = arr[p];
            int i = low;
            for (int j = low; j < high; j++) {
                if (arr[j] < pivot) {
                    std::swap(arr[i], arr[j]);
                    i++;
                }
            }
            std::swap(arr[i], arr[high - 1]);
            if (high - i > i - low) {
                if (low < i - 1) quickSortRecursive(arr, low, i - 1);
                low = i + 1;
            }
            else {
                if (i + 1 < high) quickSortRecursive(arr, i + 1, high);
                high = i - 1;
            }
        }
    }
};
}

static sortAlgo* const sortalgos[] = 
{
    new stlalgos::STLSort(),
    new stlalgos::STLStableSort(),
    new hybridalgos::timSort(),
    new hybridalgos::pdqSort<false>(),
    new nsquaredalgos::insertionSort(),
    new nsquaredalgos::selectionSort(),
    new nsquaredalgos::bubbleSort(),
    new nsquaredalgos::cocktailSort(),
    new nsquaredalgos::gnomeSort(),
    new nsquaredalgos::combSort(),
    new nlognalgos::mergeSort(),
    new nlognalgos::heapSort(),
    new nlognalgos::smoothSort(),
    new nlognalgos::shellSort(),
    new nlognalgos::bitonicSort(),
    new nlognalgos::quickSortLR<nlognalgos::firstPivot>(),
    new nlognalgos::quickSortLR<nlognalgos::lastPivot>(),
    new nlognalgos::quickSortLR<nlognalgos::middlePivot>(),
    new nlognalgos::quickSortLR<nlognalgos::medianOfThreePivot>(),
    new nlognalgos::quickSortLR<nlognalgos::medianOfNinePivot>(),
    new nlognalgos::quickSortLR<nlognalgos::randomPivot>(),
    new nlognalgos::quicksortLL<nlognalgos::firstPivot>(),
    new nlognalgos::quicksortLL<nlognalgos::lastPivot>(),
    new nlognalgos::quicksortLL<nlognalgos::middlePivot>(),
    new nlognalgos::quicksortLL<nlognalgos::medianOfThreePivot>(),
    new nlognalgos::quicksortLL<nlognalgos::medianOfNinePivot>(),
    new nlognalgos::quicksortLL<nlognalgos::randomPivot>()
};