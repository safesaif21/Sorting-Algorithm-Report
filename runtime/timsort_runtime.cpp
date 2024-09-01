// g++ -Wall -Wpedantic -march=haswell -O3 timsort_runtime.cpp -o timsort && ./timsort 100000
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
#define RUN 32

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


// This function sorts array from left 
// index to right index which is 
// of size at most RUN 
void insertionSort(DATA_T arr[], int left, int right) 
{ 
    for (int i = left + 1; i <= right; i++) { 
        DATA_T temp = arr[i]; 
        int j = i - 1; 
        while (j >= left && arr[j] > temp) { 
            arr[j + 1] = arr[j]; 
            j--; 
        } 
        arr[j + 1] = temp; 
    } 
} 
  
// Merge function merges the sorted runs 
void merge(DATA_T arr[], int l, int m, int r) 
{ 
    int len1 = m - l + 1;
    int len2 = r - m;
    DATA_T* left = (DATA_T*)malloc(len1 * sizeof(DATA_T));
    DATA_T* right = (DATA_T*)malloc(len2 * sizeof(DATA_T));
    
    for (int i = 0; i < len1; i++) 
        left[i] = arr[l + i];
    for (int i = 0; i < len2; i++) 
        right[i] = arr[m + 1 + i];
    
    int i = 0, j = 0, k = l;
    while (i < len1 && j < len2) { 
        if (left[i] <= right[j]) { 
            arr[k] = left[i]; 
            i++; 
        } 
        else { 
            arr[k] = right[j]; 
            j++; 
        } 
        k++; 
    }
    
    while (i < len1) { 
        arr[k] = left[i]; 
        k++; 
        i++; 
    }
    
    while (j < len2) { 
        arr[k] = right[j]; 
        k++; 
        j++; 
    }
    
    free(left);
    free(right);
} 
  
// Iterative Timsort function to sort the 
// array[0...n-1] (similar to merge sort) 
void timSort(DATA_T arr[], size_t n) 
{ 
    for (size_t i = 0; i < n; i += RUN) 
        insertionSort(arr, i, std::min(static_cast<size_t>(i + RUN - 1), n - 1)); 
  
    for (size_t size = RUN; size < n; size = 2 * size) { 
        for (size_t left = 0; left < n; left += 2 * size) { 
            size_t mid = left + size - 1; 
            size_t right = std::min(left + 2 * size - 1, n - 1); 
            if (mid < right) 
                merge(arr, left, mid, right); 
        } 
    } 
} 

// Function to check if the array is sorted
bool is_sorted(DATA_T* array, size_t length) {
    for (size_t i = 0; i < length - 1; i++) {
        if (array[i] > array[i + 1]) {
            return false;
        }
    }
    return true;
}

// Function to time the sorting
void time_sort(const char* descr, void(*sort)(DATA_T*, size_t), size_t length, array_ordering order) {
    struct timespec start, end;

    DATA_T* array = create_array(length, order);
    if (array == NULL) {
        printf("Couldn't allocate.\n");
        return;
    }

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    sort(array, length); // Pass the correct parameters for timSort
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
    assert(is_sorted(array, length));

    free(array);
    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("%26s sorted %8zu values in %7.2f ms\n", descr, length, elapsed * 1000);
}

// Function to test the Timsort algorithm with different orderings
void time_timsort(size_t length) {
    //time_sort("timSort on sorted", timSort, length, SORTED);
    //time_sort("timSort on reverse", timSort, length, REVERSE_SORTED);
    time_sort("timSort on random", timSort, length, RANDOM);
    //time_sort("timSort on partialy sorted", timSort, length, PARTIALLY_SORTED);
    //time_sort("timSort on many duplicates", timSort, length, MANY_DUPLICATE_VALUES);
}

// Function to sort and verify the array without timing
void just_sort(void(*sort)(DATA_T*, size_t), size_t length, array_ordering order) {
    DATA_T* array = create_array(length, order);
    if (array == NULL) {
        printf("Couldn't allocate.\n");
        return;
    }
    sort(array, length); // Pass the correct parameters for timSort
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

    size_t length = static_cast<size_t>(atol(argv[1]));
    
    // Time the Timsort
    //time_timsort(length);

    // Just sort and verify the array
    just_sort(timSort, length, RANDOM);

    return 0;
}
