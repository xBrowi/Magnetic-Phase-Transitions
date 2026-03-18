#include "../core/MonteCarlo.hpp"
#include "../core/Lattices.hpp"

#include <vector>
#include <iostream>
#include <fstream>
#include <random>
#include <chrono>

int main() 
{
    auto startTime = std::chrono::high_resolution_clock::now();

    std::ofstream outFile("output/magnetizationSize.csv");
    std::ofstream sizeFile("output/sizes.csv");

    std::vector<MCParameters> paramsList;

    for (int s = 25; s < 222; s += 7)
    {
        MCParameters params;
        params.latticeType = LatticeType::FunkySquare;
        params.size = s;
        params.temperature = 2.25;
        params.totalStepCount = 5e9;
        params.measurementInterval = 1'000'000;
        params.randomize = true;
        params.printProgress = false;

        paramsList.push_back(params);
    }

    std::vector<std::vector<Measurement>> allMeasurements = runParallelMCSimulation(paramsList);

    // Write results to CSV
    for (int i = 0; i < paramsList.size(); ++i){
        sizeFile << paramsList[i].size;
        if (i < paramsList.size() - 1) {
            sizeFile << ",";
        }
    }

    outFile << "Size,Step,Magnetization\n";
    for (size_t i = 0; i < paramsList.size(); ++i)
    {
        for (const Measurement& m : allMeasurements[i]) {
            outFile << paramsList[i].size << "," << m.step << "," << m.magnetization << "\n";
        }
    }

    outFile.close();
    sizeFile.close();

    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = endTime - startTime;
    std::cout << "Simulation completed in " << elapsed.count() << " seconds.\n";

    return 0;
}