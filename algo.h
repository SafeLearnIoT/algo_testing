#pragma once
#include <vector>
#include <array>
#include "utils.h"

namespace RTPNN {
    class SDP {
        enum Arg {
            Trends,
            Levels,
            Values,
            None
        };

        std::vector<double> m_values; // values

        std::vector<double> m_levels{0.5};
        std::vector<double> m_trends{0.5};

        std::vector<double> m_predictions;
        std::array<double, 4> m_weights{0.0, 0.0, 0.0, 0.0};

        std::vector<double> m_mse_history{};

        double m_trend_param_1 = 0.5;
        double m_trend_param_2 = 0.5;

        double m_level_param_1 = 0.5;
        double m_level_param_2 = 0.5;

        double m_min = 0.0f;
        double m_max = 0.0f;

        double m_lr = 0.001;
        double m_epochs = 100;
        int m_train_batch_size = 16;

        void calculate_trend(double &value);

        void calculate_level(double &value);

        double mse();

        double phi(double z);

        double Dphi(double z);

        double bptt(double weight, Arg arg);

        double part_sum(int i);

        void update_trend_params();

        void update_level_params();

        bool validate_size();

    public:
        SDP(SensorType sensor_type) {
            m_min = sensorConfigs.at(sensor_type).minValue;
            m_max = sensorConfigs.at(sensor_type).maxValue;
        };

        SDP(const double min, const double max, const int epochs, const double learing_rate,
            const int train_batch_size) : m_min(min), m_max(max), m_lr(learing_rate), m_epochs(epochs),
                                          m_train_batch_size(train_batch_size) {
        };

        double perform(double value, int sample_number, bool trainModel);

        double predict(double &value);

        void set_weights(std::array<double, 4> &weights);

        void train(bool print = false);

        std::vector<double> get_mse_history();
    };
}
