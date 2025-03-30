#include "algo.h"
#include <numeric>
#include <cmath>
#include <iostream>

namespace RTPNN {
    void SDP::calculate_trend(double &value) {
        m_trends.push_back((m_trend_param_1 * (value - m_values.back())) + (m_trend_param_2 * m_trends.back()));
    }

    void SDP::calculate_level(double &value) {
        m_levels.push_back((m_level_param_1 * value) + (m_level_param_2 * m_levels.back()));
    }

    double SDP::perform(double value, int sample_number, bool trainModel) {
        auto val_norm = normalize(value, m_min, m_max);
        if (!m_values.empty()) {
            calculate_level(val_norm);
            calculate_trend(val_norm);
        }
        m_values.push_back(val_norm);

        m_predictions.push_back(predict(val_norm));


        if ((m_values.size() > m_train_batch_size) && trainModel) {
            // 16
            train();
        }
        return denormalize(m_predictions.back(), m_min, m_max);
    }

    double SDP::predict(double &value) {
        return phi(
            m_weights[0] * m_trends.back() + m_weights[1] * m_levels.back() + m_weights[2] * value + m_weights[3]);
    }

    double SDP::mse() {
        double sum = 0.0;
        for (int i = 0; i < m_values.size(); i++) {
            sum += ((m_values.at(i) - m_predictions.at(i)) * (m_values.at(i) - m_predictions.at(i)));
        }
        return sum / m_values.size();
    }

    double SDP::phi(double z) {
        return 1 / (1 + std::exp(-z));
    }

    double SDP::Dphi(double z) {
        return z * (1 - z);
    }


    double SDP::bptt(double weight, Arg arg) {
        double sum = 0.0;

        int N = static_cast<int>(m_values.size());
        for (int i = 0; i < N; ++i) {
            auto calc = part_sum(i); // dL/dx

            switch (arg) {
                case Trends:
                    calc *= m_trends[i];
                    break;
                case Levels:
                    calc *= m_levels[i];
                    break;
                case Values:
                    calc *= m_values[i];
                    break;
                case None:
                    break;
            }

            sum += calc;
        }

        return weight - (m_lr * 2 / N) * sum;
    }

    double SDP::part_sum(int i) {
        return (m_values[i] - m_predictions[i]) * -1.0 * Dphi(m_predictions[i]);
    }

    void SDP::update_trend_params() {
        auto sum = 0.0;
        for (int i = 3; i < m_values.size(); ++i) {
            sum += part_sum(i) * m_weights[0] // dL/d^xk * d^xk/dt^k
                    * (
                        (m_values[i] - m_values[i - 1]) // dt^k/du^1
                        +
                        (m_trend_param_2 * (m_values[i - 1] - m_values[i - 2])) // dt^k/dt^k-1 * dl^k-1/du^1
                        +
                        (m_trend_param_2 * m_trend_param_2 * (m_values[i - 2] - m_values[i - 3]))
                        //dt^k/dt^k-1 * dt^k-1/dt^k-2 * dt^k-2/du^1
                    );
        }

        m_trend_param_1 = m_trend_param_1 - (m_lr * 2 / m_values.size()) * sum;

        sum = 0.0;
        for (int i = 3; i < m_values.size(); ++i) {
            sum += part_sum(i) * m_weights[0] // dL/d^xk * d^xk/dt^k
                    * (
                        (m_values[i] - m_values[i - 1]) // dt^k/du^2
                        +
                        (m_trend_param_2 * m_trends[i - 2]) // dt^k/dt^k-1 * dl^k-1/du^2
                        +
                        (m_trend_param_2 * m_trend_param_2 * m_trends[i - 3])
                        //dt^k/dt^k-1 * dt^k-1/dt^k-2 * dt^k-2/du^2
                    );
        }

        m_trend_param_2 = m_trend_param_2 - (m_lr * 2 / m_values.size()) * sum;
    }

    void SDP::update_level_params() {
        auto sum = 0.0;
        for (int i = 3; i < m_values.size(); ++i) {
            sum += part_sum(i) * m_weights[1] // dL/d^xk * d^xk/dl^k
                    * (
                        (m_values[i] - m_values[i - 1]) // dl^k/dv^1
                        +
                        (m_level_param_2 * m_values[i - 1]) // dl^k/dl^k-1 * dl^k-1/dv^1
                        +
                        (m_level_param_2 * m_level_param_2 * m_values[i - 2])
                        //dl^k/dl^k-1 * dl^k-1/dl^k-2 * dl^k-2/dv^1
                    );
        }
        m_level_param_1 = m_level_param_1 - (m_lr * 2 / m_values.size()) * sum;

        sum = 0.0;
        for (int i = 3; i < m_values.size(); ++i) {
            sum += part_sum(i) * m_weights[1] // dL/d^xk * d^xk/dl^k
                    * (
                        (m_values[i] - m_values[i - 1]) // dl^k/dv^2
                        +
                        (m_level_param_2 * m_levels[i - 2]) // dl^k/dl^k-1 * dl^k-1/dv^2
                        +
                        (m_level_param_2 * m_level_param_2 * m_levels[i - 3])
                        //dl^k/dl^k-1 * dl^k-1/dl^k-2 * dl^k-2/dv^2
                    );
        }
        m_level_param_2 = m_level_param_2 - (m_lr * 2 / m_values.size()) * sum;
    }

    void SDP::set_weights(std::array<double, 4> &weights) {
        m_weights = weights;
    }

    bool SDP::validate_size() {
        if (m_values.size() != m_levels.size() || m_values.size() != m_trends.size() || m_values.size() != m_predictions
            .size()) {
            return false;
        }
        return true;
    }

    void SDP::train(bool print) {
        for (int i = 0; i < m_epochs; ++i) {
            // Update weights
            m_weights[0] = bptt(m_weights[0], Arg::Trends);
            m_weights[1] = bptt(m_weights[1], Arg::Levels);
            m_weights[2] = bptt(m_weights[2], Arg::Values);
            m_weights[3] = bptt(m_weights[3], Arg::None);

            // Update trend and level parameters
            update_trend_params();
            update_level_params();

            m_predictions.clear();
            for (auto value: m_values) {
                m_predictions.push_back(predict(value));
            }

            auto mse_value = mse();
            m_mse_history.emplace_back(mse_value);

            // Update predictions
        }

        if (print) {
            std::cout << "====== TRAINING START ======" << std::endl;
            std::cout << "W1 (Trends): " << m_weights[0] << " | ";
            std::cout << "W2 (Levels): " << m_weights[1] << " | ";
            std::cout << "W3 (Values): " << m_weights[2] << " | ";
            std::cout << "W4 (1): " << m_weights[3] << std::endl;
            std::cout << "Trend param 1: " << m_trend_param_1 << " | Trend param 2: " << m_trend_param_2 << std::endl;
            std::cout << "Level param 1: " << m_level_param_1 << " | Level param 2: " << m_level_param_2 << std::endl;
            std::cout << "====== TRAINING END ======" << std::endl;
        }


        // After training, clear the vectors but leave last value
        m_values.erase(m_values.begin(), m_values.end() - 1);
        m_levels.erase(m_levels.begin(), m_levels.end() - 1);
        m_trends.erase(m_trends.begin(), m_trends.end() - 1);
        m_predictions.erase(m_predictions.begin(), m_predictions.end() - 1);
    }

    std::vector<double> SDP::get_mse_history() {
        return m_mse_history;
    }
}
