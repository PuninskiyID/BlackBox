#ifndef ANALYSISAGENT_H
#define ANALYSISAGENT_H

#include <vector>
#include <deque>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <complex>
#include <fftw3.h>
#include "common.h"
using namespace std;

class AnalysisAgent
{
public:
    AnalysisAgent();
    double* data;
    double result[X_SIZE];

    QString analysisResult;

    // Быстрое преобразование Фурье
    std::vector<std::complex<double>> fft();

    // Амплитудный спектр
    std::vector<double> magnitudeSpectrum(const std::vector<std::complex<double>>& fftResult);

    // Поиск пиков (локальных максимумов выше порога)
    std::vector<int> findPeaks(double threshold);

    // Обнаружение резких изменений (скачков)
    std::vector<int> detectEdges(double diffThreshold);

    // Обнаружение выбросов по z-оценке
    std::vector<int> detectOutliers(double zThreshold);

    std::vector<double> getFFTAmplitudes();

    double computeMean();
    double computeStdDev(double mean);

    void Analyse(const QString &arg1, double add1);
};

#endif // ANALYSISAGENT_H
