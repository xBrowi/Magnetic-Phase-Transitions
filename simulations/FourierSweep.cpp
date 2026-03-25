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
    std::vector<long int> størrelser = {24, 26, 28, 30, 32, 34, 36, 38, 42, 44, 46, 50, 52, 56, 58, 62, 66, 72, 78, 84, 90, 100, 105, 110, 118, 125, 134, 142, 154, 166, 180, 200}; // Adjust as needed
    std::vector<double> magnetiseringer = {0}; // Adjust as needed
    std::vector<double> temperaturer = {2.8, 2.9, 3.0, 3.05, 3.1, 3.15, 3.20, 3.22, 3.24, 3.26, 3.28, 3.30, 3.32, 3.34, 3.36, 3.38, 3.40, 3.42, 3.44, 3.46, 3.48, 3.5, 3.52, 3.54, 3.56, 3.58, 3.6, 3.62, 3.64, 3.66, 3.68, 3.7, 3.8, 3.9, 4.0}; // Adjust as needed
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
            for (double B : magnetiseringer)
            {
                MCParameters params;
                params.latticeType = LatticeType::Triangle;
                params.size = størrelser[i];
                params.temperature = T;
                params.B = B;
                params.totalStepCount = 100*størrelser[i]*størrelser[i]*10000;     //5e10
                params.measurementInterval = 100*størrelser[i]*størrelser[i]; //5e6
                params.randomize = true;
                params.printProgress = false;
                params.stabilizing = 0.1;
                params.stepAlgorithm = stepType::Metropolis;
                params.wolffStabilizationSteps = 0; //3 // sæt til 0 for ingen Wolff stabilisering

                paramsList.push_back(params);
            }
        } 
    }
    std::vector<MCFourierResult> allMeasurements = runParallelFourierMCSimulation(paramsList);
    
    for (size_t j = 0; j < størrelser.size(); ++j)
    {
        std::ofstream &parameterFile = parameterFiles[j];
        std::ofstream &measurementsFile = measurementsFiles[j];
        std::ofstream &FFTFile = FFTFiles[j];
        std::ofstream &FFTVarFile = FFTVarFiles[j];

        for (size_t i = temperaturer.size() * j; i < temperaturer.size() * (j+1); ++i)
        {   
            for (size_t k = 0; k < magnetiseringer.size(); ++k)
            {
                parameterFile << paramsList[i* magnetiseringer.size() + k].size << ",";
                parameterFile << paramsList[i* magnetiseringer.size() + k].temperature << ",";
                parameterFile << paramsList[i* magnetiseringer.size() + k].B << ",";
                parameterFile << paramsList[i* magnetiseringer.size() + k].totalStepCount << ",";
                parameterFile << paramsList[i* magnetiseringer.size() + k].measurementInterval<< ",";
                parameterFile << "\n";

                measurementsFile << allMeasurements[i* magnetiseringer.size() + k].count << ",";
                measurementsFile << allMeasurements[i* magnetiseringer.size() + k].magnetisering << ",";
                measurementsFile << allMeasurements[i* magnetiseringer.size() + k].magnetiseringVarians << ",";
                measurementsFile << allMeasurements[i* magnetiseringer.size() + k].magnetiseringVariansVarians << ",";
                measurementsFile << allMeasurements[i* magnetiseringer.size() + k].hamilton << ",";
                measurementsFile << allMeasurements[i* magnetiseringer.size() + k].hamiltonVarians << ",";
                measurementsFile << allMeasurements[i* magnetiseringer.size() + k].hamiltonVariansVarians << ",";
                measurementsFile << "\n";

                for (const double &m : allMeasurements[i* magnetiseringer.size() + k].normKvadrat)
                {
                    FFTFile << m << ",";
                }
                FFTFile << "\n";

                for (const double &m : allMeasurements[i* magnetiseringer.size() + k].normKvadratVarians)
                {
                    FFTVarFile << m << ",";
                }
                FFTVarFile << "\n";
            }

        }

        parameterFile.close();
        measurementsFile.close();
        FFTFile.close();
        FFTVarFile.close();
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = endTime - startTime;
    std::cout << "Simulation completed in " << elapsed.count() << " seconds.\n";

    return 0;
}