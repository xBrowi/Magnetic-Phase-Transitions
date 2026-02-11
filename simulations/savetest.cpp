#include "../core/MonteCarlo2D.hpp"
#include "../core/Lattices2D.hpp"

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

    outFile << "Temperature,Step,Magnetization,MeanClusterSize\n";

    outFile.close();
    tempFile.close();

    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = endTime - startTime;
    std::cout << "Simulation completed in " << elapsed.count() << " seconds.\n";

    return 0;
}