#include "../core/MonteCarlo2D.hpp"
#include "../core/Lattices2D.hpp"

#include <vector>
#include <iostream>
#include <fstream>
#include <random>

int main() 
{
    std::ofstream outFile("output/magnetizationSize.csv");
    std::ofstream sizeFile("output/sizes.csv");

    std::vector<MCParameters> paramsList;

    for (int s = 25; s < 126; s += 5)
    {
        MCParameters params;
        params.latticeType = LatticeType2D::FunkySquare;
        params.size = s;
        params.temperature = 2.25;
        params.totalStepCount = 2e9;
        params.measurementInterval = 10000;
        params.randomize = true;
        params.printProgress = false;

        paramsList.push_back(params);
    }

    std::vector<std::vector<measurement2D>> allMeasurements = runParallelMCSimulation(paramsList);

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
        for (const measurement2D& m : allMeasurements[i]) {
            outFile << paramsList[i].size << "," << m.step << "," << m.magnetization << "\n";
        }
    }

    outFile.close();
    sizeFile.close();

    return 0;
}