#include "MonteCarlo2D.hpp"
#include "lattices2D.hpp"

#include <vector>
#include <iostream>
#include <fstream>
#include <random>

int main() 
{
    std::ofstream outFile("magnetization.csv");
    std::ofstream tempFile("temperatures.csv");

    std::vector<MCParameters> paramsList;

    for (double T = 1.5; T <= 3; T += 0.01)
    {
        MCParameters params;
        params.latticeType = LatticeType2D::FunkySquare;
        params.size = 100;
        params.temperature = T;
        params.totalStepCount = 1e8;
        params.measurementInterval = 10000;
        params.randomize = false;
        params.printProgress = false;

        paramsList.push_back(params);
    }

    std::vector<std::vector<measurement2D>> allMeasurements = runParallelMCSimulation(paramsList);

    // Write results to CSV
    for (int i = 0; i < paramsList.size(); ++i){
        tempFile << paramsList[i].temperature;
        if (i < paramsList.size() - 1) {
            tempFile << ",";
        }
    }

    outFile << "Temperature,Step,Magnetization\n";
    for (size_t i = 0; i < paramsList.size(); ++i)
    {
        for (const measurement2D& m : allMeasurements[i]) {
            outFile << paramsList[i].temperature << "," << m.step << "," << m.magnetization << "\n";
        }
    }

    outFile.close();
    tempFile.close();

    return 0;
}