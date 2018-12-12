#include <iostream>
#include <vector>
#include "omp.h"

// MARK: - Properties
struct timespec start, finish;
double elapsed;
int quickSortLowBorder = 1000;

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
void randomArray(int* array1, int* array2, int size) {
#pragma omp for
    for (int i = 0; i < size; ++i) {
        int ran = (int)random();
        int sdf = ran % size;
        array1[i] = sdf;
        array2[i] = sdf;
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

// MARK: - QuickSort

void singleThreadQuickSort(int* array, int left, int right) {
    int i = left, j = right;
    int tmp;
    int pivot = array[(left + right) / 2]; // опорный элемент

    do {
        while (array[i] < pivot) i++;
        while (array[j] > pivot) j--;

        if (i <= j) {
            tmp = array[i];
            array[i] = array[j];
            array[j] = tmp;
            i++; j--;
        }
    } while (i <= j);

    if (left < j){ singleThreadQuickSort(array, left, j); }
    if (i < right){ singleThreadQuickSort(array, i, right); }
}

void multiThreadQuickSort(int* array, int left, int right) {

    int i = left, j = right;
    int tmp;
    int pivot = array[(left + right) / 2];

    do {
        while (array[i] < pivot) i++;
        while (array[j] > pivot) j--;

        if (i <= j) {
            tmp = array[i];
            array[i] = array[j];
            array[j] = tmp;
            i++; j--;
        }
    } while (i <= j);


    if ( ((right-left)<quickSortLowBorder) ){
        if (left < j){ multiThreadQuickSort(array, left, j); }
        if (i < right){ multiThreadQuickSort(array, i, right); }

    }else{
#pragma omp task
        { multiThreadQuickSort(array, left, j); }
#pragma omp task
        { multiThreadQuickSort(array, i, right); }
    }
//#pragma omp taskwait
}

void startMultiThreadQuickSort(int* array, int lenArray){
    int maxThreadsCount = omp_get_max_threads();
#pragma omp parallel num_threads(maxThreadsCount)
    {
#pragma omp single nowait //убирает неявную барьерную синхронизацию, которая по умолчанию стоит в конце цикла
        {
            multiThreadQuickSort(array, 0, lenArray-1);
        }
    }

}

int main() {
    int size = 50000000;
    int *array1;
    int *array2;

    array1 = (int*) malloc(size*sizeof(int));
    array2 = (int*) malloc(size*sizeof(int));

    printf("\nStart Multi thread Generate random Array");
    startMeasureTime();
    randomArray(array1, array2, size);
    finishMeasureTime("Multi thread Generating finished");

    startMeasureTime();
    singleThreadQuickSort(array1, 0, size-1);
    finishMeasureTime("Single Thread QUICK SORT ");

    startMeasureTime();
    startMultiThreadQuickSort(array2, size);
    finishMeasureTime("Multi Thread QUICK SORT");

    checkArray(array1, size);
    checkArray(array2, size);

    return 0;
}