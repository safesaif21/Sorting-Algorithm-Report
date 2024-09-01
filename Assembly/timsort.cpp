#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <cassert>
#include <algorithm>

// Define the data type and its format specifier
#define DATA_T int
#define RUN 32

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