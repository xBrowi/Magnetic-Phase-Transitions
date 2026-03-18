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

    std::ofstream outFile("output/1d_simulation.csv");

    std::vector<MCParameters> paramsList;

    for (double T = 2.25; T <= 2.35; T += 0.005)
    {
        MCParameters params;
        params.latticeType = LatticeType::OneD; // Assuming OneD for 1D lattice; adjust if needed
        params.size = 3;                        // 3-site lattice
        params.temperature = T;
        params.B = 0.0; // Default field; adjust if you want to vary it
        params.totalStepCount = 1e5;
        params.measurementInterval = 10;
        params.randomize = true;
        params.printProgress = false;
        // Assuming free boundary conditions; add params.boundaryCondition = BoundaryType::Free; if available

        paramsList.push_back(params);
    }

    std::vector<std::vector<Measurement>> allMeasurements = runParallelMCSimulation(paramsList);

    // Write results to CSV
    outFile << "Size,Temperature,Field,Step,Magnetization,Energy\n";
    for (size_t i = 0; i < paramsList.size(); ++i)
    {
        for (const Measurement &m : allMeasurements[i])
        {
            outFile << paramsList[i].size << "," << paramsList[i].temperature << "," << paramsList[i].B << "," << m.step << "," << m.magnetization << "," << m.energy << "\n";
        }
    }

    outFile.close();

    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = endTime - startTime;
    std::cout << "Simulation completed in " << elapsed.count() << " seconds.\n";

    return 0;
}