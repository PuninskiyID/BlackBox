#include "filteragent.h"
#include <vector>
#include <deque>
#include <cmath>
#include <algorithm>
#include <numeric>

FilterAgent::FilterAgent()
{

}

std::array<double,X_SIZE> FilterAgent::movingAverage(int windowSize) {
    std::array<double,X_SIZE> output;
    if (windowSize <= 0 || X_SIZE < windowSize) return output;

    std::deque<double> window;
    double sum = 0.0;
    for (size_t i = 0; i < X_SIZE; ++i) {
        window.push_back(data[i]);
        sum += data[i];
        if (window.size() > windowSize) {
            sum -= window.front();
            window.pop_front();
        }
        output[i] = sum / window.size();
    }
    return output;
}

std::array<double,X_SIZE> FilterAgent::medianFilter(int windowSize) {
    std::array<double,X_SIZE> output;
    int halfWindow = windowSize / 2;
    for (size_t i = 0; i < X_SIZE; ++i) {
        std::vector<double> window;
        for (int j = static_cast<int>(i) - halfWindow; j <= static_cast<int>(i) + halfWindow; ++j) {
            if (j >= 0 && j < static_cast<int>(X_SIZE))
                window.push_back(data[j]);
        }
        std::sort(window.begin(), window.end());
        output[i] = window[window.size() / 2];
    }
    return output;
}

std::array<double,X_SIZE> FilterAgent::exponentialSmoothing(double alpha) {
    std::array<double,X_SIZE> output;
    //if (input.empty()) return output;
    double y_prev = data[0];
    output[0] = y_prev;
    for (size_t i = 1; i < X_SIZE; ++i) {
        double y = alpha * y_prev + (1.0 - alpha) * data[i];
        output[i] = y;
        y_prev = y;
    }
    return output;
}

std::array<double,X_SIZE> FilterAgent::highPassFilter(double alpha) {
    std::array<double,X_SIZE> lowPassed = exponentialSmoothing(alpha);
    std::array<double,X_SIZE> highPassed;
    for (size_t i = 0; i < X_SIZE; ++i) {
        highPassed[i] = data[i] - lowPassed[i];
    }
    return highPassed;
}

std::array<double,X_SIZE> FilterAgent::bandPassFilter(double alphaLow, double alphaHigh) {
    auto highPassed = highPassFilter(alphaLow);
    return exponentialSmoothing(alphaHigh);
}

std::array<double,X_SIZE> FilterAgent::outlierRemoval(double thresholdZ)
{
    std::array<double,X_SIZE> output;
    std::copy(data, data + X_SIZE, output.begin());
    double mean = computeMean();
    double stddev = computeStdDev(mean);
    for (size_t i = 0; i < X_SIZE; ++i) {
        double z = (data[i] - mean) / stddev;
        if (std::abs(z) > thresholdZ) {
            // Заменяем медианой из окна [i-2, i+2]
            std::vector<double> window;
            for (int j = static_cast<int>(i) - 2; j <= static_cast<int>(i) + 2; ++j) {
                if (j >= 0 && j < static_cast<int>(X_SIZE)) window.push_back(data[j]);
            }
            std::sort(window.begin(), window.end());
            output[i] = window[window.size() / 2];
        }
    }
    return output;
}

void FilterAgent::Filter(const QString &arg1, double add1=0.1, double add2=0.2)
{
    std::array<double, X_SIZE> res;
    if (arg1 == "MovingAverage")
    {
        res = movingAverage(add1);
    }
    else if (arg1 == "MedianFilter") {
        res = medianFilter(add1);
    }
    else if (arg1 == "ExponentialSmoothing") {
        res = exponentialSmoothing(add1);
    }
    else if (arg1 == "HighPassFilter") {
        res = highPassFilter(add1);
    }
    else if (arg1 == "BandPassFilter") {
        res = bandPassFilter(add1, add2);
    }
    else if (arg1 == "OutlierRemoval") {
        res = outlierRemoval(add1);
    }
    std::copy(res.begin(), res.end(), result);
}

double FilterAgent::computeMean() {
    return std::accumulate(data, data + X_SIZE, 0.0) / X_SIZE;
}

double FilterAgent::computeStdDev(double mean) {
    double sum = 0.0;
    double x;
    for (int i=0; i<X_SIZE; i++) {
        x = data[i];
        sum += (x - mean) * (x - mean);
    }
    return std::sqrt(sum / X_SIZE);
}



