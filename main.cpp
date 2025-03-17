#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <cmath>
#include "algo.h"

struct EnvData {
    double temperature;
    double pressure;
    double humidity;
    double iaq;

    [[nodiscard]] std::string toString() const {
        return '[' + std::to_string(temperature) + " " + std::to_string(pressure) + " " + std::to_string(humidity) + " " + std::to_string(iaq) + ']';
    }
};

std::vector<EnvData> readCsv(std::string filename) {
    std::vector<EnvData> data;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return data;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::vector<std::string> row;
        std::stringstream ss(line);
        std::string cell;

        while (std::getline(ss, cell, ',')) {
            row.push_back(cell);
        }

        data.push_back(EnvData {
            std::round(atof(row[0].c_str())*10.0)/10.0,
            std::round(atof(row[1].c_str())*10.0)/10.0,
            std::round(atof(row[2].c_str())*10.0)/10.0,
            std::round(atof(row[3].c_str())*10.0)/10.0,
        });
    }

    file.close();

    return data;
};

void printData(const std::vector<EnvData> &envData, const int n) {
    for (int i = 0; i < (envData.size() > n ? n : envData.size()); i++) {
        std::cout << envData[i].toString() << std::endl;
    }
};

double getMax(const std::vector<EnvData> &envData) {
    auto max = 0.0;
    for (const auto el : envData) {
        if (el.temperature > max) {
            max = el.temperature;
        }
    }
    return max;
}
double getMin(const std::vector<EnvData> &envData) {
    auto min = 1000.0;
    for (const auto el : envData) {
        if (el.temperature < min) {
            min = el.temperature;
        }
    }
    return min;
}

int main() {
    auto data = readCsv("../output_outside_dummy_const_5_data.csv");
    //auto data = readCsv("../output_inside_dummy_const_15_data.csv");
    //auto data = readCsv("../output_inside_dummy_const_5_data.csv");

    int learningSize = static_cast<int>(std::round(data.size()*0.7));
    double mape_learn = 0;
    double mape_test = 0;
    double mape_whole = 0;

    auto min = getMin(data);
    auto max = getMax(data);


    int epochs = 100; // best 70
    double learning_rate = 0.05; // best 0.05

    RTPNN::SDP temperature{min, max, epochs, learning_rate};

    int i = 0;
    double pred = 0;
    for (auto el : data) {
        auto abs = std::abs(el.temperature-pred);
        std::cout << i << ". Reading: " << el.temperature << " | Prediction:" << pred << " | Difference: " << abs << std::endl;
        auto mape_val = abs/std::abs(el.temperature);
        mape_whole += mape_val;
        if (i > learningSize) {
            mape_test += mape_val;
        } else {
            mape_learn += mape_val;

        }
        pred = std::round(temperature.perform(el.temperature, i, i <= learningSize )*10)/10;

        ++i;
    }

    std::cout << "Min: " << min << " | Max: " << max << std::endl;
    std::cout << "Whole MAPE: " << (mape_whole/data.size())*100 << "%" << std::endl;
    std::cout << "Train MAPE: " << (mape_learn/learningSize)*100 << "%" << std::endl;
    std::cout << "Test MAPE: " << (mape_test/(data.size()-learningSize))*100 << "%" << std::endl;

    return 0;
}


