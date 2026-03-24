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

    std::ofstream parameterFile("output/Fourier1D/parameters.csv");
    std::ofstream measurementsFile("output/Fourier1D/measurements.csv");
    std::ofstream FFTFile("output/Fourier1D/kvadratFFT.csv");
    std::ofstream FFTVarFile("output/Fourier1D/kvadratFFTvariance.csv");


    std::vector<MCParameters> paramsList;

    for (double T = 0.4; T <= 4.05; T += 0.1)
    {
        MCParameters params;
        params.latticeType = LatticeType::OneD;
        params.size = 2048;
        params.temperature = T;
        params.B = 0.0;
        params.totalStepCount = 1e8;
        params.measurementInterval = 1e5;
        params.randomize = false;
        params.printProgress = false;
        params.stabilizing = 0.2;
        params.stepAlgorithm = stepType::Metropolis;
        params.wolffStabilizationSteps = 5; // sæt til 0 for ingen Wolff stabilisering

        paramsList.push_back(params);
    }

    std::vector<MCFourierResult> allMeasurements = runParallelFourierMCSimulation(paramsList);

    for (size_t i = 0; i < paramsList.size(); ++i)
    {   
        parameterFile << paramsList[i].size << ",";
        parameterFile << paramsList[i].temperature << ",";
        parameterFile << paramsList[i].B << ",";
        parameterFile << paramsList[i].totalStepCount << ",";
        parameterFile << paramsList[i].measurementInterval<< ",";
        parameterFile << "\n";

        measurementsFile << allMeasurements[i].count << ",";
        measurementsFile << allMeasurements[i].magnetisering << ",";
        measurementsFile << allMeasurements[i].magnetiseringVarians << ",";
        measurementsFile << allMeasurements[i].magnetiseringVariansVarians << ",";
        measurementsFile << allMeasurements[i].hamilton << ",";
        measurementsFile << allMeasurements[i].hamiltonVarians << ",";
        measurementsFile << allMeasurements[i].hamiltonVariansVarians << ",";
        measurementsFile << "\n";

        for (const double &m : allMeasurements[i].normKvadrat)
        {
            FFTFile << m << ",";
        }
        FFTFile << "\n";

        for (const double &m : allMeasurements[i].normKvadratVarians)
        {
            FFTVarFile << m << ",";
        }
        FFTVarFile << "\n";

    }

    parameterFile.close();
    measurementsFile.close();
    FFTFile.close();
    FFTVarFile.close();

    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = endTime - startTime;
    std::cout << "Simulation completed in " << elapsed.count() << " seconds.\n";

    return 0;
}