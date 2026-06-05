# sortbench
Command-line benchmark for various sorting algorithms

## Usage
First you need to compile `main.cpp` into an executable:
```sh
g++ -O2 -std=c++17 -o sortbench main.cpp
```
Then you can execute like this:
```sh
./sortbench [-a algo]... [-g gen]... [-o outputfile] @filename
```
### Options
* `-a` / `--algo`: Algorithms to benchmark
* `-g` / `--gen`: Generators to generate the array
* `-o` / `--out`: Output file
* `@filename`: The file containing array sizes to test on
* Remaining numerical arguments: Array sizes to test on
### Tips
* `@filename` can be replaced with several numbers on the command line.
* `[-a algo]...` has the following three ways to use:
    * `-a algo1,algo2,...`
    * `-a=algo1,algo2,...`
    * `-a algo1 -a algo2 ...`
* `[-g gen]...` is use in the same way as `[-a algo]`. The only difference is that this option specifies the generators to use instead of the algorithms to use.
### Outputs
By default, this program outputs a Markdown table that uses LaTeX formatting for numerical cells. You can redirect the stream through `-o`.

For example, the following command could produce this table:
```sh
./sortbench --algo pdq --gen adversary 500000 5000000
```
| data size | PDQ-sort |
|:-:|:-:|
| 500000(adversary) | $12.589474\text{ms}(6230477,17187767)$ |
| 5000000(adversary) | $97.140569\text{ms}(74523469,190760965)$ |

In the first column, the number outside the bracket is array length, and the text inside the bracket refers to the array generation method.

In any other column, the numbers in the bracket means the comparison and the array access count the sorting algorithm used.

## Algorithms & Generators
|Acronym (used in command lines)|Description|Time complexity|
|:-:|:-:|:-:|
|stlsort|std::sort|$O(n\log n)$|
|stlstable|std::stable_sort|$O(n\log n)$|
|tim|Timsort|$O(n\log n)$|
|pdq|PDQ-sort|$O(n\log n)$|
|insertion|Insertion sort|$O(n^2)$|
|selection|Selection sort|$O(n^2)$|
|bubble|Bubble sort|$O(n^2)$|
|cocktail|Cocktail sort|$O(n^2)$|
|gnome|Gnome sort|$O(n^2)$|
|comb|Comb sort|Unknown|
|merge|Merge sort|$O(n\log n)$|
|heap|Heap sort|$O(n\log n)$|
|smooth|Smooth sort|$O(n\log n)$|
|shell|Shell sort|$O(n^{\frac32})$|
|bitonic|Bitonic sort|$O(n\log^2 n)$|
|quicklrfirst|Quick sort (LR, pivot=first)|$O(n\log n)$ but $O(n^2)$ worst|
|quicklrlast|Quick sort (LR, pivot=last)|$O(n\log n)$ but $O(n^2)$ worst|
|quicklrmid|Quick sort (LR, pivot=middle)|$O(n\log n)$ but $O(n^2)$ worst|
|quicklrmed3|Quick sort (LR, pivot=median of 3)|$O(n\log n)$ but $O(n^2)$ worst|
|quicklrmed9|Quick sort (LR, pivot=median of 9)|$O(n\log n)$ but $O(n^2)$ worst|
|quicklrrand|Quick sort (LR, pivot=random)|$O(n\log n)$ but $O(n^2)$ worst|
|quickllfirst|Quick sort (LL, pivot=first)|$O(n\log n)$ but $O(n^2)$ worst|
|quicklllast|Quick sort (LL, pivot=last)|$O(n\log n)$ but $O(n^2)$ worst|
|quickllmid|Quick sort (LL, pivot=middle)|$O(n\log n)$ but $O(n^2)$ worst|
|quickllmed3|Quick sort (LL, pivot=median of 3)|$O(n\log n)$ but $O(n^2)$ worst|
|quickllmed9|Quick sort (LL, pivot=median of 9)|$O(n\log n)$ but $O(n^2)$ worst|
|quickllrand|Quick sort (LL, pivot=random)|$O(n\log n)$ but $O(n^2)$ worst|

|Acronym (used in command lines)|Description|
|:-:|:-:|
|rand|Random|
|asc|Ascending|
|desc|Descending|
|pipeorgan|Pipe organ|
|merge|$[n/2,n)$ ascending followed by $[0,n/2)$ ascending|
|mergerev|Previous one reversed|
|heap|Max-heap array|
|bst|$\log_2n$ runs in increasing length|
|saw|4-crest sawtooth|
|randhead|Scrambled head|
|randtail|Scrambled tail|
|halfrev|$[0,n/2)$ ascending followed by $[n/2,n)$ descending|
|adversary|[M.D. Mcilroy's adversarial array](https://www.cs.dartmouth.edu/~doug/mdmspe.pdf)|

## Contribution
Anyone is free to contribute, but remember to use pull requests and wait for my approval instead of directly pushing onto the branch.
### Coding guideline
Apart from a few exceptions listed below, the code in this repository mainly follows the LLVM style formatting with 4 spaces for indentation.
* Opening braces (`{`) must be in the same line with the last closing parenthesis (`)`).
    * They should have one space in between when no keywords are between them.
* Only use `auto` when you are declaring a Lambda expression, an iterator or the variable of a range-based for-loop. Otherwise specify which types you are using.
* No `decltype` is allowed.
* Existing code uses lower camel case for historical reasons. New code may use either lower camel case or PascalCase for class names, but should be consistent within a single file.
* Write self-documenting code instead of comments. Class names and function names should explain everything clearly.
### Use of AI
AI-generated code is **prohibited**, but their warnings and tips are welcome. If you use AI, you must specify the parts of the workflow AI participated in. **If I think a commit contains AI-generated code, I will require you to close them.**
