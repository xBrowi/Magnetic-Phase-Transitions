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
    std::vector<MCParameters> paramsList;

    MCParameters params;

    params.latticeType = LatticeType::Triangle;
    params.size = 200;
    params.temperature = 3.45;
    params.B = 0.0;
    params.totalStepCount = 1e9;
    params.measurementInterval = 1e6; //magic number 50 for all wolffs
    params.randomize = true;
    params.printProgress = false;
    params.stabilizing = 0.0;
    params.stepAlgorithm = stepType::Metropolis;
    params.wolffStabilizationSteps = 0; // sæt til 0 for ingen Wolff stabilisering

    paramsList.push_back(params);
            
    std::vector<MCFourierResult> allMeasurements = runParallelFourierMCSimulation(paramsList);
    

    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = endTime - startTime;
    std::cout << "Simulation completed in " << elapsed.count() << " seconds.\n";

    return 0;
}