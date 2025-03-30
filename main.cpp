#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <cmath>
#include "algo.h"
#include "test.h"


int main() {
    //auto data = readCsv("../output_outside_dummy_const_5_data.csv");
    auto insideData = readEnvdataCsv("../esp8266_inside.csv");
    auto outsideData = readEnvdataCsv("../esp8266_outside.csv");
    auto windowData = readWindowCsv("../esp32_window.csv");
    auto lightData = readLightData("../esp32_light.csv");

    auto epochs = {
        5, 10, 20, 30, 35, 40, 45, 50, 55, 60, 65, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160, 170, 180, 200
    };
    auto learning_rates = {.005, .01, .05, .06, .07, .1, .2, .3, .4, .5, .6, .7};
    auto train_batch_sizes = {7, 8, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 25, 30, 50, 60, 100};
    //auto epochs = {130};
    //auto learning_rates = {.5};
    //auto train_batch_sizes = {20};

    std::vector<Configuration> configs;
    for (auto e: epochs) {
        for (auto lr: learning_rates) {
            for (auto tr: train_batch_sizes) {
                configs.emplace_back(e, lr, tr);
            }
        }
    }

    auto envLabels = std::vector{"Temperature", "Pressure", "Humidity", "IAQ"};
    auto windowLabels = std::vector{"Blinds1", "Blinds2", "Window"};

    for (int i = 0; i < insideData.size(); i++) {
        std::cout << ">> " << envLabels.at(i) << " IN" << std::endl;
        Test test(insideData.at(i), configs);
        test.runAllTests();
        auto result = test.getBestResult();
        result.printResult();
    }

    for (int i = 0; i < outsideData.size(); i++) {
        std::cout << ">> " << envLabels.at(i) << " OUT" << std::endl;
        Test test(outsideData.at(i), configs);
        test.runAllTests();
        auto result = test.getBestResult();
        result.printResult();
    }

    for (int i = 0; i < windowData.size(); i++) {
        std::cout << ">> " << windowLabels.at(i) << std::endl;
        Test test(windowData.at(i), configs);
        test.runAllTests();
        auto result = test.getBestResult();
        result.printResult();
    }

    std::cout << ">> Light" << std::endl;
    Test test(lightData, configs);
    test.runAllTests();
    auto result = test.getBestResult();
    result.printResult();

    return 0;
}


