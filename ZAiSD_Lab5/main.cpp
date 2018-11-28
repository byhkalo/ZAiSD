#include <iostream>
#include "omp.h"

//#include <algorithm>
//#include <cstddef>
//#include <iterator>
//#include <memory>

// MARK: - Properties
struct timespec start, finish;
double elapsed;
// MARK: - Measuring Time Methods
void startMeasureTime() {
    clock_gettime(CLOCK_MONOTONIC, &start);
}
void finishMeasureTime(const char * addingString) {
    clock_gettime(CLOCK_MONOTONIC, &finish);
    elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    printf("\n%s Elapsed time (s) = %.8f", addingString, elapsed);
}
// MARK: - Array Methods
void randomArray(int array[], int size) {
    for (int i = 0; i < size; ++i) {
        int ran = (int)random();
        int sdf = ran % size;
        array[i] = sdf;
    }
}
void checkArray(int array[], int size) {
    for (int i = 1; i < size; ++i) {
        int previousIndex = i-1;
        if (array[previousIndex] > array[i]) {
            printf("\nArray has issue | index = %d", i);
            return;
        }
    }
    printf("\nArray is Correct");
}
void printArray(int array[], int size) {
    printf("\n\n START Print array with size = %d", size);
    for (int i = 0; i < size; ++i) {
        int element = array[i];
        printf("\nelement = %d", element);
    }
    printf("\n\n END Print array");
}

//MARK: - MergeSort

int* singleThreadMergeSort(int *up, int *down, unsigned int left, unsigned int right)
{
    if (left == right)
    {
        down[left] = up[left];
        return down;
    }

    unsigned int middle = (left + right) / 2;

    // разделяй и властвуй
    int *l_buff = singleThreadMergeSort(up, down, left, middle);
    int *r_buff = singleThreadMergeSort(up, down, middle + 1, right);

    // слияние двух отсортированных половин
    int *target = l_buff == up ? down : up;

    unsigned int l_cur = left, r_cur = middle + 1;
    for (unsigned int i = left; i <= right; i++)
    {
        if (l_cur <= middle && r_cur <= right)
        {
            if (l_buff[l_cur] < r_buff[r_cur])
            {
                target[i] = l_buff[l_cur];
                l_cur++;
            }
            else
            {
                target[i] = r_buff[r_cur];
                r_cur++;
            }
        }
        else if (l_cur <= middle)
        {
            target[i] = l_buff[l_cur];
            l_cur++;
        }
        else
        {
            target[i] = r_buff[r_cur];
            r_cur++;
        }
    }
    return target;
}
int* startSingleThreadMergeSort(int* array, const long size) {
    int* sortedArray;
    int bufferArray[size];
    sortedArray = singleThreadMergeSort(array, bufferArray, 0, (unsigned int) size - 1);
    return sortedArray;
}



int* multiThreadMergeSort(int *up, int *down, unsigned int left, unsigned int right)
{
    if (left == right)
    {
        down[left] = up[left];
        return down;
    }

    unsigned int middle = (left + right) / 2;

    // разделяй и властвуй
    int *l_buff;
    int *r_buff;
#pragma omp task shared(up, down) firstprivate(middle, left)
    {
        l_buff = multiThreadMergeSort(up, down, left, middle);
    }
#pragma omp task shared(up, down) firstprivate(middle, right)
    {
        r_buff = multiThreadMergeSort(up, down, middle + 1, right);
    }

#pragma omp taskwait
    // слияние двух отсортированных половин
    int *target = l_buff == up ? down : up;

    unsigned int l_cur = left, r_cur = middle + 1;
    for (unsigned int i = left; i <= right; i++)
    {
        if (l_cur <= middle && r_cur <= right)
        {
            if (l_buff[l_cur] < r_buff[r_cur])
            {
                target[i] = l_buff[l_cur];
                l_cur++;
            }
            else
            {
                target[i] = r_buff[r_cur];
                r_cur++;
            }
        }
        else if (l_cur <= middle)
        {
            target[i] = l_buff[l_cur];
            l_cur++;
        }
        else
        {
            target[i] = r_buff[r_cur];
            r_cur++;
        }
    }
    return target;
}

int* startMultiThreadMergeSort(int* array, const long size) {
    int* sortedArray;
#pragma omp parallel shared(array)
    {
#pragma omp single nowait
        {
            int bufferArray[size];
            sortedArray = multiThreadMergeSort(array, bufferArray, 0, (unsigned int) size - 1);
        } // #pragma omp single
    }
    return sortedArray;
}

// MARK: - QuickSort
void singleThreadQuickSort(int* a, const long n) {
    long i = 0, j = n;
    float	pivot = a[n / 2]; // опорный элемент

    do {
        while (a[i] < pivot) i++;
        while (a[j] > pivot) j--;

        if (i <= j) {
            std::swap(a[i], a[j]);
            i++; j--;
        }
    } while (i <= j);

    if (j > 0) singleThreadQuickSort(a, j);
    if (n > i) singleThreadQuickSort(a + i, n - i);
}



void multiThreadQuickSort(int* a, const long n) {
    long i = 0, j = n;
    float	pivot = a[n / 2]; // опорный элемент


    do {
        while (a[i] < pivot) i++;
        while (a[j] > pivot) j--;

        if (i <= j) {
            std::swap(a[i], a[j]);
            i++; j--;
        }
    } while (i <= j);

#pragma omp task shared(a)
    {
        if (j > 0) multiThreadQuickSort(a, j);
    } // #pragma omp task
#pragma omp task shared(a)
    {
        if (n > i) multiThreadQuickSort(a + i, n - i);
    } // #pragma omp task
#pragma omp taskwait
}

void startMultiThreadQuickSort(int* array, const long size) {
#pragma omp parallel shared(array)
    {
#pragma omp single nowait
        {
            multiThreadQuickSort(array, size - 1);
        } // #pragma omp single
    }
}


int main() {
    int size = 10000;
    int array[size];
    int array2[size];
    int array3[size];
    int array4[size];

    randomArray(array, size);

    for (int i = 0; i < size; ++i) {
        array2[i] = array[i];
        array3[i] = array[i];
        array4[i] = array[i];
    }
    randomArray(array2, size);

    checkArray(array, size);

    startMeasureTime();
    singleThreadQuickSort(array, size-1);
    finishMeasureTime("Single Thread QUICK SORT ");

    startMeasureTime();
    startMultiThreadQuickSort(array2, size);
    finishMeasureTime("Multi Thread QUICK SORT");

    startMeasureTime();
    int *sortedSingleThread = startSingleThreadMergeSort(array3, size);
    finishMeasureTime("Single Thread MERGE SORT ");

//    startMeasureTime();
//    int *sortedMultiThread = startMultiThreadMergeSort(array4, size);
//    finishMeasureTime("Multi Thread MERGE SORT");


    checkArray(array, size);
    checkArray(array2, size);
    checkArray(sortedSingleThread, size);
//    checkArray(sortedMultiThread, size);

    return 0;
}