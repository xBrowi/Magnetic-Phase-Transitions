//#include "../core/ClusterAnalysis.hpp"
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

    std::ofstream outFile("output/fourier_sweep_output.csv");

    std::vector<MCParameters> paramsList;

    for (double T = 2; T <= 4; T += 0.2)
    {
        for (double B = -1.0; B <= 1.1; B += 0.25)
        {
            MCParameters params;
            params.latticeType = LatticeType::FunkySquare;
            params.size = 20;
            params.temperature = T;
            params.B = B;
            params.totalStepCount = 1e6;
            params.measurementInterval = 1'000;
            params.randomize = true;
            params.printProgress = false;
            paramsList.push_back(params);
        }
    }

    std::vector<std::vector<double>> allMeasurements = runParallelFourierMCSimulation(paramsList);

    outFile << "paramsliste, gennemsnitlige koefficient norm for hver simulation\n";
    for (size_t i = 0; i < paramsList.size(); ++i)
    {   
        outFile << paramsList[i].size << ",";
        outFile << paramsList[i].temperature << ",";
        outFile << paramsList[i].B << ",";
        outFile << paramsList[i].totalStepCount << ",";
        outFile << paramsList[i].measurementInterval<< ",";

        for (const double &m : allMeasurements[i])
        {
            outFile << m << ",";
        }
        outFile << "\n";
    }

    outFile.close();

    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = endTime - startTime;
    std::cout << "Simulation completed in " << elapsed.count() << " seconds.\n";

    return 0;
}