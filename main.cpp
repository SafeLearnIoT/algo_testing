#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
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
            atof(row[0].c_str()),
            atof(row[1].c_str()),
            atof(row[2].c_str()),
            atof(row[3].c_str()),
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

    auto min = getMin(data);
    auto max = getMax(data);
    std::cout << "Min: " << min << " | Max: " << max << std::endl;

    int epochs = 70;
    double learning_rate = 0.05;

    RTPNN::SDP temperature{min, max, epochs, learning_rate};

    int i = 0;
    double pred = 0;
    for (auto el : data) {
        std::cout << i << ". Reading: " << el.temperature << " | Prediction:" << pred << " | Difference: " << std::abs(el.temperature-pred) << std::endl;
        pred = temperature.perform(el.temperature, i);
        ++i;
    }
    return 0;
}


