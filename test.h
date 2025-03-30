#pragma once

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "algo.h"

std::vector<std::vector<double> > readEnvdataCsv(const std::string &filename);

std::vector<std::vector<int> > readWindowCsv(const std::string &filename);

std::vector<long> readLightData(const std::string &filename);

template<class T>
double getMaxData(const std::vector<T> &data) {
    auto max = std::numeric_limits<T>::min();
    for (const auto el: data) {
        if (el > max) {
            max = el;
        }
    }
    return max;
}


template<class T>
double getMinData(const std::vector<T> &data) {
    auto min = std::numeric_limits<T>::max();
    for (const auto el: data) {
        if (el < min) {
            min = el;
        }
    }
    return min;
}

template<class T>
struct Step {
    T value;
    double prediction;

    Step(T value, const double prediction) : value(value), prediction(prediction) {
    };
};

struct Configuration {
    int epochs;
    double learning_rate;
    int train_batch_size;

    Configuration(const int epochs, const double learning_rate, const int train_batch_size) : epochs(epochs),
        learning_rate(learning_rate), train_batch_size(train_batch_size) {
    };
};

template<class T>
struct Result {
    double mapeWhole{};
    double mapeTrain{};
    double mapeTest{};

    std::vector<double> mseHistory{};

    std::vector<Step<T> > steps;
    Configuration configuration;
    int learningSetSize{};

    explicit Result(const Configuration &config, const int lrSize) : configuration(config), learningSetSize(lrSize) {
    };

    void printMseHistory() {
        std::cout << "MSE history:" << std::endl;
        for (auto i = 0; i < mseHistory.size(); i += 1) {
            std::cout << "[" << i << "] " << mseHistory[i] << std::endl;
        }
    }

    void printResult() const {
        std::cout << "Whole MAPE: " << (mapeWhole / steps.size()) * 100 << "%" << std::endl;
        std::cout << "Train MAPE: " << (mapeTrain / learningSetSize) * 100 << "%" << std::endl;
        std::cout << "Test MAPE: " << (mapeTest / (steps.size() - learningSetSize)) * 100 << "%" << std::endl;
        std::cout << "=========================================" << std::endl;
        std::cout << "Epochs: " << configuration.epochs << std::endl;
        std::cout << "Learning rate: " << configuration.learning_rate << std::endl;
        std::cout << "Training batch size: " << configuration.train_batch_size << std::endl;
        std::cout << std::endl << std::endl;
    };
};

template<class T>
class Test {
    std::vector<Configuration> m_configs;
    std::vector<T> m_data;
    std::vector<Result<T> > m_results;

    double m_min;
    double m_max;

    int m_learning_set_size;

public:
    Test(const std::vector<T> &data, const std::vector<Configuration> &config) : m_configs(config), m_data(data),
        m_min(getMinData<T>(data)), m_max(getMaxData<T>(data)),
        m_learning_set_size(static_cast<int>(std::round(m_data.size() * 0.7))) {
    }

    Result<T> getBestResult() const {
        Result<T> bestResult = m_results.at(0);
        for (int i = 1; i < m_results.size(); ++i) {
            if (bestResult.mapeTest > m_results.at(i).mapeTest) {
                bestResult = m_results.at(i);
            }
        }
        return bestResult;
    }

    void runAllTests() {
        for (const auto config: m_configs) {
            run(config);
        }
    }

    void run(Configuration config) {
        Result<T> result{config, m_learning_set_size};

        RTPNN::SDP rtpnn{m_min, m_max, config.epochs, config.learning_rate, config.train_batch_size};
        int i = 0;
        double pred = 0;

        for (auto el: m_data) {
            result.steps.push_back(Step<T>{el, pred});
            auto abs = std::abs(el - pred);
            auto mape_val = abs / std::abs(el);
            result.mapeWhole += mape_val;
            if (i > m_learning_set_size) {
                result.mapeTest += mape_val;
            } else {
                result.mapeTrain += mape_val;
            }

            pred = std::round(rtpnn.perform(static_cast<double>(el), i, i <= m_learning_set_size) * 10) / 10;
            ++i;
        }
        result.mseHistory = rtpnn.get_mse_history();
        m_results.emplace_back(result);
    }
};
