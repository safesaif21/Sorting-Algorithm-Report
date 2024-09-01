// g++ -Wall -Wpedantic -march=haswell -O3 merge_sort_runtime.cpp  && ./a.out 100000
// Get modern behaviour out of time.h, per https://stackoverflow.com/a/40515669
#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <cassert>

// Define the data type and its format specifier
#define DATA_T int
#define DATA_PRINTF "%d"
#define RAND_EXPR (rand() % 256 - 128)

// Enum for array ordering
enum array_ordering { RANDOM, SORTED, REVERSE_SORTED, ALMOST_SORTED, PARTIALLY_SORTED, MANY_DUPLICATE_VALUES };

// Function to shuffle the array a little
void gently_shuffle_array(DATA_T* array, size_t length) {
    size_t limit = 10;
    for (size_t i = 0; i < length - limit - 1; i++) {
        size_t offset = rand() % limit;
        DATA_T tmp = array[i + offset];
        array[i + offset] = array[i];
        array[i] = tmp;
    }
}

// Function to create the array based on the ordering
DATA_T* create_array(size_t length, array_ordering order) {
    DATA_T* array = (DATA_T*)malloc(length * sizeof(DATA_T));
    if (array == NULL) {
        return NULL;
    }

    switch (order) {
    case RANDOM:
        for (size_t i = 0; i < length; i++) {
            array[i] = RAND_EXPR;
        }
        break;
    case SORTED:
        for (size_t i = 0; i < length; i++) {
            array[i] = RAND_EXPR;
        }
        std::sort(array, array + length);
        break;
    case ALMOST_SORTED:
        for (size_t i = 0; i < length; i++) {
            array[i] = RAND_EXPR;
        }
        std::sort(array, array + length);
        gently_shuffle_array(array, length);
        break;
    case REVERSE_SORTED:
        for (size_t i = 0; i < length; i++) {
            array[i] = RAND_EXPR;
        }
        std::sort(array, array + length);
        std::reverse(array, array + length);
        break;
    case PARTIALLY_SORTED:
        for (size_t i = 0; i < length; i++) {
            array[i] = RAND_EXPR;
        }
        std::sort(array, array + length / 2); // Sort only the first half
        break;
    case MANY_DUPLICATE_VALUES:
        for (size_t i = 0; i < length; i++) {
            array[i] = RAND_EXPR;
        }
        // Make many values the same
        DATA_T value = array[0];
        for (size_t i = 0; i < length / 2; i++) {
            array[i] = value;
        }
        break;
    }

    return array;
}

// Merges two subarrays of array[].
// First subarray is arr[begin..mid]
// Second subarray is arr[mid+1..end]
void merge(DATA_T* array, uint64_t const left, uint64_t const mid, uint64_t const right) {
    int const subArrayOne = mid - left + 1;
    int const subArrayTwo = right - mid;

    // Create temp arrays
    auto *leftArray = new DATA_T[subArrayOne];
    auto *rightArray = new DATA_T[subArrayTwo];

    // Copy data to temp arrays leftArray[] and rightArray[]
    for (int i = 0; i < subArrayOne; i++)
        leftArray[i] = array[left + i];
    for (int j = 0; j < subArrayTwo; j++)
        rightArray[j] = array[mid + 1 + j];

    int indexOfSubArrayOne = 0, indexOfSubArrayTwo = 0;
    int indexOfMergedArray = left;

    // Merge the temp arrays back into array[left..right]
    while (indexOfSubArrayOne < subArrayOne && indexOfSubArrayTwo < subArrayTwo) {
        if (leftArray[indexOfSubArrayOne] <= rightArray[indexOfSubArrayTwo]) {
            array[indexOfMergedArray] = leftArray[indexOfSubArrayOne];
            indexOfSubArrayOne++;
        } else {
            array[indexOfMergedArray] = rightArray[indexOfSubArrayTwo];
            indexOfSubArrayTwo++;
        }
        indexOfMergedArray++;
    }

    // Copy the remaining elements of left[], if there are any
    while (indexOfSubArrayOne < subArrayOne) {
        array[indexOfMergedArray] = leftArray[indexOfSubArrayOne];
        indexOfSubArrayOne++;
        indexOfMergedArray++;
    }

    // Copy the remaining elements of right[], if there are any
    while (indexOfSubArrayTwo < subArrayTwo) {
        array[indexOfMergedArray] = rightArray[indexOfSubArrayTwo];
        indexOfSubArrayTwo++;
        indexOfMergedArray++;
    }
    delete[] leftArray;
    delete[] rightArray;
}

// begin is for left index and end is right index
// of the sub-array of arr to be sorted
void merge_sort(DATA_T* array, uint64_t const begin, uint64_t const end) {
    if (begin >= end)
        return;

    int mid = begin + (end - begin) / 2;
    merge_sort(array, begin, mid);
    merge_sort(array, mid + 1, end);
    merge(array, begin, mid, end);
}

// Function to check if the array is sorted
bool is_sorted(DATA_T* array, uint64_t length) {
    for (uint64_t i = 0; i < length - 1; i++) {
        if (array[i] > array[i + 1]) {
            return false;
        }
    }
    return true;
}

// Function to time the sorting
void time_sort(const char* descr, void(*sort)(DATA_T*, uint64_t, uint64_t), uint64_t length, array_ordering order) {
    struct timespec start, end;

    DATA_T* array = create_array(length, order);
    if (array == NULL) {
        printf("Couldn't allocate.\n");
        return;
    }

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    sort(array, 0, length - 1); // Pass the correct parameters for mergeSort
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
    assert(is_sorted(array, length));

    free(array);
    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("%29s sorted %8lu values in %7.2f ms\n", descr, length, elapsed * 1000);
}

// Function to test the merge sort algorithm with different orderings
void time_merge_sort(uint64_t length) {
    //time_sort("merge_sort on sorted", merge_sort, length, SORTED);
    //time_sort("merge_sort on reverse", merge_sort, length, REVERSE_SORTED);
    time_sort("merge_sort on random", merge_sort, length, RANDOM);
    //time_sort("merge_sort on partialy sorted", merge_sort, length, PARTIALLY_SORTED);
   //time_sort("merge_sort on many duplicates", merge_sort, length, MANY_DUPLICATE_VALUES);
}

// Function to sort and verify the array without timing
void just_sort(void(*sort)(DATA_T*, uint64_t, uint64_t), uint64_t length, array_ordering order) {
    DATA_T* array = create_array(length, order);
    if (array == NULL) {
        printf("Couldn't allocate.\n");
        return;
    }
    sort(array, 0, length - 1); // Pass the correct parameters for mergeSort
    assert(is_sorted(array, length));
    free(array);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Must give array size on command line.\n");
        return 1;
    }
    
    uint64_t n = atol(argv[1]);
    printf("Array size: %lukB\n", n * sizeof(DATA_T) / 1024);
    
    uint64_t length = atol(argv[1]);
    
    // Time the merge sort
    //time_merge_sort(length);

    // Just sort and verify the array
    just_sort(merge_sort, length, RANDOM);

    return 0;
}
