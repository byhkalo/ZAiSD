#include <iostream>
#include "omp.h"
#include <math.h>

struct timespec start, finish;
double elapsed;

void startMeasureTime() {
    clock_gettime(CLOCK_MONOTONIC, &start);
}

void finishMeasureTime(const char * addingString) {
    clock_gettime(CLOCK_MONOTONIC, &finish);
    elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    printf("\n%s Elapsed time (s) = %.8f", addingString, elapsed);
}
// MARK: - Pi Calculating
double func(double x) {
    return 1.0/(1.0 + x * x);
}

void multiThreadImplementation() {
    startMeasureTime();
    int maxThreadsCount = omp_get_max_threads();
    double variables[maxThreadsCount];
#pragma omp parallel
    {
        int kernelNumber = omp_get_thread_num();
        int threadsCount = omp_get_num_threads();
        double deltaX = 0.000001;
        int sliceCount = (int)(1.0/deltaX);
        double squareSum = 0;
        for (int i = kernelNumber; i < sliceCount; i += threadsCount) {
            double x = i * deltaX;
            squareSum += func(x) * deltaX;
        }
        variables[kernelNumber] = squareSum;
    }

    double totalSum = 0;
    for (int kernelIndex = 0; kernelIndex < maxThreadsCount; ++kernelIndex) {
        totalSum += variables[kernelIndex];
    }

    double pi = totalSum * 4;
    printf("\nMulti treading result Computed PI %.4f", pi);
    finishMeasureTime("Multi Treading");
}

void multiThreadWithReductionImplementation() {
    startMeasureTime();
    double deltaX = 0.000001;
    int sliceCount = (int)(1.0/deltaX);
    double sum = 0;
    #pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < sliceCount; i++) {
        sum += func(i * deltaX) * deltaX;
    }

    double pi = sum * 4;
    printf("\nMulti treading REDUCTION result Computed PI %.4f", pi);
    finishMeasureTime("Multi Treading REDUCTION");
}

void singleThreadImplementation() {
    startMeasureTime();
    double deltaX = 0.000001;
    int sliceCount = (int)(1.0/deltaX);
    double squareSum = 0;
    for (int i = 0; i < sliceCount; i++) {
        double x = i * deltaX;
        squareSum += func(x) * deltaX;
    }
    double pi = squareSum * 4 ;
    printf("\nSingle treading result Computed PI %.4f", pi);
    finishMeasureTime("Single Treading");
}

// MARK: - Pi Calculating With rect and circle

void singleThreadCalculateSquaresAndPI() {
    startMeasureTime();
    int innerCircleCount = 0;//число точек, попавших в круг
    int pointsCount = 50000;//общее число точек
    double x, y;//координаты
    double pi;//результат
    long radius = 600;

    for(int i = 0; i < pointsCount; i++)
    {
        x = (random() % radius);
        y = (random() % radius);
        if (pow(x, 2) + pow(y, 2) <= pow(radius, 2)) {
            innerCircleCount++;
        }
    }

    pi = (double)4*innerCircleCount/pointsCount;
    double rectSquare = 4 * pow(radius, 2);
    double circleSquare = pi * pow(radius, 2);
    printf("\nGenerated Pi: %lg", pi);
    printf("\nrectSquare = : %.f", rectSquare);
    printf("\ncircleSquare = : %.f", circleSquare);
    finishMeasureTime("Finding Pi in coordinate finished");
}

void multiThreadCalculateSquaresAndPI() {
    startMeasureTime();

    int innerCircleCount = 0;
    int pointsCount = 50000;
    double x, y, pi;
    long radius = 600;

    #pragma omp parallel for reduction(+:innerCircleCount)
    for(int i = 0; i < pointsCount; i++)
    {
        x = (random() % radius);
        y = (random() % radius);
        if (pow(x, 2) + pow(y, 2) <= pow(radius, 2)) {
            innerCircleCount++;
        }
    }

    pi = (double)4*innerCircleCount/pointsCount;
    double rectSquare = 4 * pow(radius, 2);
    double circleSquare = pi * pow(radius, 2);
    printf("\nGenerated Pi: %lg", pi);
    printf("\nrectSquare = : %.f", rectSquare);
    printf("\ncircleSquare = : %.f", circleSquare);
    finishMeasureTime("Multi Threading Finding Pi in coordinate finished");
}

int main() {
    printf("\n\n\n");
    multiThreadImplementation();
    printf("\n\n\n");
    multiThreadWithReductionImplementation();
    printf("\n\n\n");
    singleThreadImplementation();
    printf("\n\n\n");
    singleThreadCalculateSquaresAndPI();
    printf("\n\n\n");
    multiThreadCalculateSquaresAndPI();
    printf("\n\n\n");
    return 0;
}
