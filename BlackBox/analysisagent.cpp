#include "analysisagent.h"
#include <algorithm>
#include <numeric>
#include <fftw3.h>

AnalysisAgent::AnalysisAgent()
{

}

std::vector<std::complex<double>> AnalysisAgent::fft() {
    std::vector<std::complex<double>> result(X_SIZE);
    fftw_complex* in = reinterpret_cast<fftw_complex*>(fftw_malloc(sizeof(fftw_complex) * X_SIZE));
    fftw_complex* out = reinterpret_cast<fftw_complex*>(fftw_malloc(sizeof(fftw_complex) * X_SIZE));

    for (int i = 0; i < X_SIZE; ++i) {
        in[i][0] = data[i];
        in[i][1] = 0.0;
    }
    fftw_plan plan = fftw_plan_dft_1d(X_SIZE, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(plan);
    for (int i = 0; i < X_SIZE; ++i) {
        result[i] = std::complex<double>(out[i][0], out[i][1]);
    }
    fftw_destroy_plan(plan);
    fftw_free(in);
    fftw_free(out);
    return result;
}

std::vector<double> AnalysisAgent::magnitudeSpectrum(const std::vector<std::complex<double>>& fftResult) {
    std::vector<double> magnitudes;
    for (const auto& val : fftResult) {
        magnitudes.push_back(std::abs(val));
    }
    return magnitudes;
}

std::vector<int> AnalysisAgent::findPeaks(double threshold) {
    std::vector<int> peaks;
    for (size_t i = 1; i < X_SIZE - 1; ++i) {
        if (data[i] > data[i - 1] && data[i] > data[i + 1] && data[i] > threshold) {
            peaks.push_back(static_cast<int>(i));
        }
    }
    return peaks;
}

std::vector<int> AnalysisAgent::detectEdges(double diffThreshold) {
    std::vector<int> edges;
    for (size_t i = 1; i < X_SIZE; ++i) {
        double diff = data[i] - data[i - 1];
        if (std::abs(diff) > diffThreshold) {
            edges.push_back(static_cast<int>(i));
        }
    }
    return edges;
}

std::vector<int> AnalysisAgent::detectOutliers(double zThreshold) {
    std::vector<int> outliers;
    double mean = computeMean();
    double stddev = computeStdDev(mean);
    for (size_t i = 0; i < X_SIZE; ++i) {
        double z = (data[i] - mean) / stddev;
        if (std::abs(z) > zThreshold) {
            outliers.push_back(static_cast<int>(i));
        }
    }
    return outliers;
}

std::vector<double> AnalysisAgent::getFFTAmplitudes() {
    std::vector<std::complex<double>> spectrum = fft();
    std::vector<double> amplitudes(X_SIZE / 2);

    for (int i = 0; i < X_SIZE / 2; ++i) {
        amplitudes[i] = std::abs(spectrum[i]); // модуль комплексного числа
    }

    return amplitudes;
}

double AnalysisAgent::computeMean() {
    return std::accumulate(data, data + X_SIZE, 0.0) / X_SIZE;
}

double AnalysisAgent::computeStdDev(double mean) {
    double sum = 0.0;
    double x;
    for (int i=0; i<X_SIZE; i++) {
        x = data[i];
        sum += (x - mean) * (x - mean);
    }
    return std::sqrt(sum / X_SIZE);
}

void AnalysisAgent::Analyse(const QString &arg1, double add1)
{
    analysisResult.clear();
    std::vector<double> res;

    if (arg1 == "FFT")
    {
        res = getFFTAmplitudes();
    }
    else if (arg1 == "Magnitude") {
        std::vector<std::complex<double>> spec = fft();
        res = magnitudeSpectrum(spec);
    }
    else if (arg1 == "Peaks") {
        std::vector<int> peaks = findPeaks(add1);
        res.resize(X_SIZE, 0.0);
        analysisResult += "Peaks : ";
        for (int index : peaks) {
            if (index >= 0 && index < X_SIZE)
            {
                analysisResult+=QString::number(data[index]) + " ";
                res[index] = data[index];  // можно метку, амплитуду или 1.0
            }
        }
    }
    else if (arg1 == "Edges") {
        std::vector<int> edges = detectEdges(add1);
        res.resize(X_SIZE, 0.0);
        for (int index : edges) {
            if (index >= 0 && index < X_SIZE)
                res[index] = data[index];
        }
    }
    else if (arg1 == "Outliers") {
        std::vector<int> outliers = detectOutliers(add1);
        res.resize(X_SIZE, 0.0);
        for (int index : outliers) {
            if (index >= 0 && index < X_SIZE)
                res[index] = data[index];
        }
    }
    else if (arg1 == "Mean") {
        double mean = computeMean();
        analysisResult += "Mean : " + QString::number(mean);
        res.resize(X_SIZE, mean);  // нарисуем горизонтальную линию
    }
    else if (arg1 == "StandardDeviation") {
        double stddev = computeStdDev(computeMean());
        analysisResult += "Deviation : " + QString::number(stddev);
        res.resize(X_SIZE, stddev);
    }

    // Переносим результат в обычный массив
    std::fill(result, result + X_SIZE, 0.0);
    for (int i = 0; i < std::min((int)res.size(), X_SIZE); ++i) {
        result[i] = res[i];
    }
}


