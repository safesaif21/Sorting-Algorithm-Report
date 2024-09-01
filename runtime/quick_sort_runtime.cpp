// g++ -Wall -Wpedantic -march=haswell -O3 quick_sort_runtime.cpp -o quick_sort && ./quick_sort 100000
// Get modern behavior out of time.h, per https://stackoverflow.com/a/40515669
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
        perror("Failed to allocate memory");
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

int partition(DATA_T arr[], int start, int end) {
    DATA_T pivot = arr[start];
    int count = 0;
    for (int i = start + 1; i <= end; i++) {
        if (arr[i] <= pivot)
            count++;
    }
    int pivotIndex = start + count;
    std::swap(arr[pivotIndex], arr[start]);
    int i = start, j = end;
    while (i < pivotIndex && j > pivotIndex) {
        while (i < pivotIndex && arr[i] <= pivot) {
            i++;
        }
        while (j > pivotIndex && arr[j] > pivot) {
            j--;
        }
        if (i < pivotIndex && j > pivotIndex) {
            std::swap(arr[i++], arr[j--]);
        }
    }
    return pivotIndex;
}

void quickSort(DATA_T arr[], uint64_t start, uint64_t end) {
    if (start >= end)
        return;
    int p = partition(arr, start, end);
    quickSort(arr, start, p - 1);
    quickSort(arr, p + 1, end);
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
    sort(array, 0, length - 1); // Pass the correct parameters for quickSort
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
    assert(is_sorted(array, length));

    free(array);
    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("%29s sorted %8lu values in %7.2f ms\n", descr, length, elapsed * 1000);
}

// Function to test the quick sort algorithm with different orderings
void time_quick_sort(uint64_t length) {
    //time_sort("quick_sort on sorted", quickSort, length, SORTED);
    //time_sort("quick_sort on reverse", quickSort, length, REVERSE_SORTED);
    time_sort("quick_sort on random", quickSort, length, RANDOM);
    //time_sort("quick_sort on partially sorted", quickSort, length, PARTIALLY_SORTED);
    //time_sort("quick_sort on many duplicates", quickSort, length, MANY_DUPLICATE_VALUES);
}

// Function to sort and verify the array without timing
void just_sort(void(*sort)(DATA_T*, uint64_t, uint64_t), uint64_t length, array_ordering order) {
    DATA_T* array = create_array(length, order);
    if (array == NULL) {
        printf("Couldn't allocate.\n");
        return;
    }
    sort(array, 0, length - 1); // Pass the correct parameters for quickSort
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
    
    // Time the quick sort
    //time_quick_sort(length);

    // Just sort and verify the array
    just_sort(quickSort, length, RANDOM);

    return 0;
}
