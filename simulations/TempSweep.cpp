#include "../core/MonteCarlo.hpp"
#include "../core/Lattices.hpp"

#include <vector>
#include <iostream>
#include <fstream>
#include <random>
#include <chrono>

int main()
{
    // time the simulation
    auto startTime = std::chrono::high_resolution_clock::now();

    std::ofstream outFile("output/magnetization.csv");
    std::ofstream tempFile("output/temperatures.csv");
    std::ofstream fieldFile("output/fields.csv");

    std::vector<MCParameters> paramsList;

    for (double T = 1; T <= 4.1; T += 0.5)
    {
        for (double B = -1.0; B <= 1.1; B += 0.5)
        {
            MCParameters params;
            params.latticeType = LatticeType::FunkySquare;
            params.size = 80;
            params.temperature = T;
            params.B = B;
            params.totalStepCount = 5e6;
            params.measurementInterval = 10'000;
            params.randomize = true;
            params.printProgress = false;
            paramsList.push_back(params);
        }
    }

    // for (double T = 2.25; T <= 2.35; T += 0.005)
    // {
    //     MCParameters params;
    //     params.latticeType = LatticeType::FunkySquare;
    //     params.size = 200;
    //     params.temperature = T;
    //     params.totalStepCount = 5e9;
    //     params.measurementInterval = 1'000'000;
    //     params.randomize = true;
    //     params.printProgress = false;

    //     paramsList.push_back(params);
    // }

    std::vector<std::vector<Measurement>> allMeasurements = runParallelMCSimulation(paramsList);

    // Write results to CSV
    for (int i = 0; i < paramsList.size(); ++i)
    {
        tempFile << paramsList[i].temperature;
        if (i < paramsList.size() - 1)
        {
            tempFile << ",";
        }
    }

    for (int i = 0; i < paramsList.size(); ++i)
    {
        fieldFile << paramsList[i].B;
        if (i < paramsList.size() - 1)
        {
            fieldFile << ",";
        }
    }

    outFile << "Temperature,ExternalField,Step,Magnetization,MeanClusterSize\n";
    for (size_t i = 0; i < paramsList.size(); ++i)
    {
        for (const Measurement &m : allMeasurements[i])
        {
            outFile << paramsList[i].temperature << "," << paramsList[i].B << "," << m.step << "," << m.magnetization << "," << m.meanClusterSize << "\n";
        }
    }

    outFile.close();
    tempFile.close();

    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = endTime - startTime;
    std::cout << "Simulation completed in " << elapsed.count() << " seconds.\n";

    return 0;
}