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

    // std::ofstream parameterFile("output/Fourier/parameters.csv");
    //std::ofstream measurementsFile("output/Fourier/measurements.csv");
    //std::ofstream FFTFile("output/Fourier/kvadratFFT.csv");
    //std::ofstream FFTVarFile("output/Fourier/kvadratFFTvariance.csv");


    //på 6 timer har vi 2.16e13 steps
    std::vector<int> størrelser = {24, 26, 28, 30, 32, 34, 36, 38, 42, 44, 46, 50, 52, 56, 58, 62, 66, 72, 78, 84, 90, 100, 105, 110, 118, 125, 134, 142, 154, 166, 180, 200}; // Adjust as needed
    std::vector<double> temperaturer = {2.26, 2.261, 2.262, 2.263, 2.264, 2.265, 2.266, 2.267, 2.268, 2.269, 2.27, 2.271, 2.272, 2.273, 2.274, 2.275, 2.276, 2.277, 2.278, 2.279, 2.28}; // Adjust as needed
    //std::vector<int> størrelser = {50, 40, 32}; // Adjust as needed
    
    int totalSimulations = størrelser.size() * temperaturer.size();
    std::cout << "Running " << totalSimulations << " simulations...\n";

    std::vector<std::ofstream> parameterFiles(størrelser.size()); // 4 filer per størrelse
    std::vector<std::ofstream> measurementsFiles(størrelser.size());
    std::vector<std::ofstream> FFTFiles(størrelser.size());
    std::vector<std::ofstream> FFTVarFiles(størrelser.size());

    std::vector<MCParameters> paramsList;

    for (int i = 0; i < størrelser.size(); ++i) {
        parameterFiles[i] = std::ofstream("output/Fourier/parameters/parameters_" + std::to_string(størrelser[i]) + ".csv");
        measurementsFiles[i] = std::ofstream("output/Fourier/measurements/measurements_" + std::to_string(størrelser[i]) + ".csv");
        FFTFiles[i] = std::ofstream("output/Fourier/kvadratFFT/kvadratFFT_" + std::to_string(størrelser[i]) + ".csv");
        FFTVarFiles[i] = std::ofstream("output/Fourier/kvadratFFTvariance/kvadratFFTvariance_" + std::to_string(størrelser[i]) + ".csv");

        for (double T : temperaturer)
        {
            MCParameters params;
            params.latticeType = LatticeType::FunkySquare;
            params.size = størrelser[i];
            params.temperature = T;
            params.B = 0.0;
            params.totalStepCount = 5e5;     //5e10
            params.measurementInterval = 50; //5e6
            params.randomize = true;
            params.printProgress = false;
            params.stabilizing = 0.1;
            params.stepAlgorithm = stepType::Wolff;
            params.wolffStabilizationSteps = 1; //3 // sæt til 0 for ingen Wolff stabilisering

            paramsList.push_back(params);
        } 
    }
    std::vector<MCFourierResult> allMeasurements = runParallelFourierMCSimulation(paramsList);
    
    for (size_t j = 0; j < størrelser.size(); ++j)
        for (size_t i = temperaturer.size() * j; i < temperaturer.size() * (j+1); ++i)
        {   
            std::ofstream &parameterFile = parameterFiles[j];
            std::ofstream &measurementsFile = measurementsFiles[j];
            std::ofstream &FFTFile = FFTFiles[j];
            std::ofstream &FFTVarFile = FFTVarFiles[j];

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

    //parameterFile.close();
    //measurementsFile.close();
    //FFTFile.close();
    //FFTVarFile.close();
    for (size_t i = 0; i < parameterFiles.size(); ++i)
    {
        parameterFiles[i].close();
        measurementsFiles[i].close();
        FFTFiles[i].close();
        FFTVarFiles[i].close();
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = endTime - startTime;
    std::cout << "Simulation completed in " << elapsed.count() << " seconds.\n";

    return 0;
}