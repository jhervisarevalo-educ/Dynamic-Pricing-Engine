// ============================================================================
//  sort-algo/sort_algorithms.hpp
//  ---------------------------------------------------------------------------
//  A small library of sorting algorithms for the Sales Ranking System.
//
//  Every algorithm sorts a std::vector<Product> in DESCENDING order of
//  unitsSold (best-sellers first), so they can all be compared fairly.
//
//  The algorithms are grouped in the `sortalgo` namespace and all functions
//  are marked `inline` because this is a header-only library.
//
//  A registry (allSortAlgorithms) pairs each algorithm with a display name and
//  its Big-O, which lets the menu build a chooser and the benchmark run them all.
// ============================================================================
#ifndef SORT_ALGORITHMS_HPP
#define SORT_ALGORITHMS_HPP

#include <vector>
#include <string>
#include "../product.hpp"

namespace sortalgo {

// ---------------------------------------------------------------------------
//  Bubble Sort
//  Repeatedly swaps adjacent out-of-order items so the largest "bubbles" up.
//  Time: O(n^2) worst/average, O(n) best (already sorted). Stable.
// ---------------------------------------------------------------------------
inline void bubbleSort(std::vector<Product>& products) {
    std::size_t n = products.size();
    for (std::size_t i = 0; i + 1 < n; ++i) {
        bool swapped = false;
        // After each pass, the smallest remaining item settles at the end.
        for (std::size_t j = 0; j + 1 < n - i; ++j) {
            if (products[j].unitsSold < products[j + 1].unitsSold) {
                std::swap(products[j], products[j + 1]);
                swapped = true;
            }
        }
        if (!swapped) {
            break; // no swaps means the list is already sorted (best case O(n))
        }
    }
}

// ---------------------------------------------------------------------------
//  Selection Sort
//  Each pass selects the largest remaining item and places it next.
//  Time: O(n^2) in all cases. Not stable.
// ---------------------------------------------------------------------------
inline void selectionSort(std::vector<Product>& products) {
    std::size_t n = products.size();
    for (std::size_t i = 0; i + 1 < n; ++i) {
        std::size_t maxIndex = i;
        for (std::size_t j = i + 1; j < n; ++j) {
            if (products[j].unitsSold > products[maxIndex].unitsSold) {
                maxIndex = j;
            }
        }
        if (maxIndex != i) {
            std::swap(products[i], products[maxIndex]);
        }
    }
}

// ---------------------------------------------------------------------------
//  Insertion Sort
//  Builds the sorted list one item at a time by inserting each item into place.
//  Time: O(n^2) worst/average, O(n) best (already sorted). Stable.
// ---------------------------------------------------------------------------
inline void insertionSort(std::vector<Product>& products) {
    for (std::size_t i = 1; i < products.size(); ++i) {
        Product key = products[i];
        // Shift larger-or-equal-position items right until the spot is found.
        // (We want descending order, so we move items with SMALLER unitsSold.)
        std::size_t j = i;
        while (j > 0 && products[j - 1].unitsSold < key.unitsSold) {
            products[j] = products[j - 1];
            --j;
        }
        products[j] = key;
    }
}

// ---------------------------------------------------------------------------
//  Merge Sort (divide and conquer)
//  Time: O(n log n) in ALL cases. Space: O(n). Stable.
// ---------------------------------------------------------------------------

// Merges two already-sorted halves [left..mid] and [mid+1..right] so that
// the combined range is sorted in DESCENDING order of unitsSold.
inline void mergeHalves(std::vector<Product>& products, int left, int mid, int right) {
    // Copy the two halves into temporary vectors.
    std::vector<Product> leftHalf(products.begin() + left,
                                   products.begin() + mid + 1);
    std::vector<Product> rightHalf(products.begin() + mid + 1,
                                    products.begin() + right + 1);

    std::size_t i = 0; // index into leftHalf
    std::size_t j = 0; // index into rightHalf
    int k = left;      // index into the original vector

    // Pick the larger unitsSold first (descending order).
    while (i < leftHalf.size() && j < rightHalf.size()) {
        if (leftHalf[i].unitsSold >= rightHalf[j].unitsSold) {
            products[k++] = leftHalf[i++];
        } else {
            products[k++] = rightHalf[j++];
        }
    }

    // Copy any remaining elements from the left half.
    while (i < leftHalf.size()) {
        products[k++] = leftHalf[i++];
    }
    // Copy any remaining elements from the right half.
    while (j < rightHalf.size()) {
        products[k++] = rightHalf[j++];
    }
}

// Recursively splits the range in half, sorts each half, then merges them.
// Time complexity: O(n log n).
inline void mergeSortRange(std::vector<Product>& products, int left, int right) {
    if (left >= right) {
        return; // a range of 0 or 1 element is already sorted
    }
    int mid = left + (right - left) / 2;    // avoids potential overflow
    mergeSortRange(products, left, mid);      // sort left half
    mergeSortRange(products, mid + 1, right); // sort right half
    mergeHalves(products, left, mid, right);  // combine the two sorted halves
}

// Public entry point with the uniform signature used by the registry.
inline void mergeSort(std::vector<Product>& products) {
    if (!products.empty()) {
        mergeSortRange(products, 0, static_cast<int>(products.size()) - 1);
    }
}

// ---------------------------------------------------------------------------
//  Quick Sort (divide and conquer)
//  Time: O(n log n) average, O(n^2) worst (bad pivots). Space: O(log n). Not stable.
// ---------------------------------------------------------------------------

// Lomuto partition around the last element, arranging for DESCENDING order.
inline int partition(std::vector<Product>& products, int low, int high) {
    int pivot = products[high].unitsSold;
    int i = low - 1;
    for (int j = low; j < high; ++j) {
        // For descending order, move larger items to the front.
        if (products[j].unitsSold > pivot) {
            ++i;
            std::swap(products[i], products[j]);
        }
    }
    std::swap(products[i + 1], products[high]);
    return i + 1;
}

inline void quickSortRange(std::vector<Product>& products, int low, int high) {
    if (low < high) {
        int p = partition(products, low, high);
        quickSortRange(products, low, p - 1);
        quickSortRange(products, p + 1, high);
    }
}

// Public entry point with the uniform signature used by the registry.
inline void quickSort(std::vector<Product>& products) {
    if (!products.empty()) {
        quickSortRange(products, 0, static_cast<int>(products.size()) - 1);
    }
}

// ---------------------------------------------------------------------------
//  Registry: pairs each algorithm with a name, Big-O, and function pointer.
//  Also flags whether an algorithm is quadratic, so callers can skip the slow
//  ones on very large datasets.
// ---------------------------------------------------------------------------
struct SortAlgo {
    std::string name;
    std::string bigO;
    void (*run)(std::vector<Product>&);
    bool isQuadratic; // true for O(n^2) algorithms
};

inline std::vector<SortAlgo> allSortAlgorithms() {
    return {
        {"Bubble Sort",    "O(n^2)",     bubbleSort,    true},
        {"Selection Sort", "O(n^2)",     selectionSort, true},
        {"Insertion Sort", "O(n^2)",     insertionSort, true},
        {"Merge Sort",     "O(n log n)", mergeSort,     false},
        {"Quick Sort",     "O(n log n)", quickSort,     false},
    };
}

} // namespace sortalgo

#endif // SORT_ALGORITHMS_HPP
