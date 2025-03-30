#pragma once
#include <vector>

template<class T>
struct Step {
    T value;
    T prediction;
};

struct Configuration {
    int epochs;
    double learning_rate;
    double train_batch_size;

    Configuration(const int epochs, const double learning_rate, const double train_batch_size): epochs(epochs),
        learning_rate(learning_rate), train_batch_size(train_batch_size) {
    };
};

template<class T>
class Test {
    std::vector<Configuration> configurations;
    std::vector<Step<T>> steps;
    //std::vector<Step<T>> steps;
};
