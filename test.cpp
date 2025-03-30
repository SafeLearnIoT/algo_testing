#include "test.h"

#include <fstream>
#include <cmath>
#include <iostream>
#include <sstream>
#include <limits>
#include <string>

std::vector<std::vector<double>> readEnvdataCsv(const std::string &filename)
{
    std::vector<double> temperature;
    std::vector<double> pressure;
    std::vector<double> humidity;
    std::vector<double> iaq;

    std::ifstream file(filename);

    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return {};
    }

    std::string line;
    while (std::getline(file, line))
    {
        std::vector<std::string> row;
        std::stringstream ss(line);
        std::string cell;

        while (std::getline(ss, cell, ','))
        {
            row.push_back(cell);
        }

        temperature.push_back(std::round(atof(row[0].c_str()) * 10.0) / 10.0);
        pressure.push_back(std::round(atof(row[1].c_str()) * 10.0) / 10.0);
        humidity.push_back(std::round(atof(row[2].c_str()) * 10.0) / 10.0);
        iaq.push_back(std::round(atof(row[3].c_str()) * 10.0) / 10.0);
    }

    file.close();

    return {temperature, pressure, humidity, iaq};
}

std::vector<std::vector<int>> readWindowCsv(const std::string &filename)
{
    std::vector<int> blinds_1;
    std::vector<int> blinds_2;
    std::vector<int> window;
    std::ifstream file(filename);

    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return {};
    }

    std::string line;
    while (std::getline(file, line))
    {
        std::vector<std::string> row;
        std::stringstream ss(line);
        std::string cell;

        while (std::getline(ss, cell, ','))
        {
            row.push_back(cell);
        }

        blinds_1.push_back(atoi(row[0].c_str()));
        blinds_2.push_back(atoi(row[1].c_str()));
        window.push_back(atoi(row[2].c_str()));
    }

    file.close();

    return {blinds_1, blinds_2, window};
}

std::vector<long> readLightData(const std::string &filename)
{
    std::vector<long> data;
    std::ifstream file(filename);

    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return data;
    }

    std::string line;
    while (std::getline(file, line))
    {
        std::vector<std::string> row;
        std::stringstream ss(line);
        std::string cell;

        while (std::getline(ss, cell, ','))
        {
            row.push_back(cell);
        }

        data.push_back(
            std::stol(row[0].c_str()));
    }

    file.close();

    return data;
}