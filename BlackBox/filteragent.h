#ifndef FILTERAGENT_H
#define FILTERAGENT_H

#include <vector>
#include "common.h"
using namespace std;

class FilterAgent
{
public:
    FilterAgent();
    double* data;
    double result[X_SIZE];

    // Скользящее среднее
    std::array<double,X_SIZE> movingAverage(int windowSize);

    // Медианный фильтр
    std::array<double,X_SIZE> medianFilter(int windowSize);

    // Экспоненциальное сглаживание (IIR)
    std::array<double,X_SIZE> exponentialSmoothing(double alpha);

    // Фильтр верхних частот: x[n] - LPF(x[n])
    std::array<double,X_SIZE> highPassFilter(double alpha);

    // Полосовой фильтр: HPF -> LPF (каскадно)
    std::array<double,X_SIZE> bandPassFilter(double alphaLow, double alphaHigh);

    // Медианный выброс-фильтр: заменяет выброс медианой
    std::array<double,X_SIZE> outlierRemoval(double thresholdZ);

    double computeMean();
    double computeStdDev(double mean);

    void Filter(const QString &arg1, double add1, double add2);
};

#endif // FILTERAGENT_H
