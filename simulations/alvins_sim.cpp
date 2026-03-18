// this script runs a simple 1d lattice and calculates M, E, chi and Cv for a range of temperatures.

#include "../core/MonteCarlo.hpp"
#include "../core/Lattices.hpp"
#include <vector>
#include <iostream>

struct Averages
{
    double M = 0.0;
    double M2 = 0.0;
    double E = 0.0;
    double E2 = 0.0;
    int count = 0;

    void add(const Measurement &m)
    {
        M += m.magnetization;
        M2 += m.magnetization * m.magnetization;
        E += m.energy;
        E2 += m.energy * m.energy;
        count++;
    }
};

double susceptibility(const Averages &avg, double beta, int N)
{
    double M_avg = avg.M / avg.count;
    double M2_avg = avg.M2 / avg.count;

    return beta * N * (M2_avg - M_avg * M_avg);
}

double specificHeat(const Averages &avg, double beta, int N)
{
    double E_avg = avg.E / avg.count;
    double E2_avg = avg.E2 / avg.count;

    return beta * beta * (E2_avg - E_avg * E_avg) / N;
}

double averageEnergy(const Averages &avg)
{
    return avg.E / avg.count;
}

int main()
{
    // time the simulation
    auto startTime = std::chrono::high_resolution_clock::now();

    std::ofstream outFile("output/alvin_magnetization.csv");
    std::ofstream tempFile("output/alvin_temperatures.csv");
    std::ofstream fieldFile("output/alvin_fields.csv");

    std::vector<MCParameters> paramsList;

    for (double T = 2.25; T <= 2.35; T += 0.005)
    {
        MCParameters params;
        params.latticeType = LatticeType::FunkySquare;
        params.size = 200;
        params.temperature = T;
        params.totalStepCount = 1e5;
        params.measurementInterval = 10;
        params.randomize = true;
        params.printProgress = false;

        paramsList.push_back(params);
    }

    std::vector<std::vector<Measurement>> allMeasurements = runParallelMCSimulation(paramsList);

    // New code to compute custom averages per temperature
    std::vector<Averages> customAverages(paramsList.size());
    for (size_t i = 0; i < paramsList.size(); ++i) {
        for (const Measurement &m : allMeasurements[i]) {
            customAverages[i].add(m);
        }
    }

    // Write results to CSV
    for (int i = 0; i < paramsList.size(); ++i)
    {
        tempFile << paramsList[i].temperature;
        if (i < paramsList.size() - 1)
        {
            tempFile << ",";
        }
    }

    for (int i = 0; i < paramsList.size(); ++i)
    {
        fieldFile << paramsList[i].B;
        if (i < paramsList.size() - 1)
        {
            fieldFile << ",";
        }
    }

    outFile << "Temperature,ExternalField,Step,Magnetization,Energy,Susceptibility,SpecificHeat\n";
    for (size_t i = 0; i < paramsList.size(); ++i)
    {
        for (const Measurement &m : allMeasurements[i])
        {
            outFile << paramsList[i].temperature << "," << paramsList[i].B << "," << m.step << "," << m.magnetization << "," << m.energy << "," << m.susceptibility << "," << m.specificHeat << "\n";
        }
    }

    // Output custom computed values (e.g., append to CSV or print)
    for (size_t i = 0; i < paramsList.size(); ++i) {
        double beta = 1.0 / paramsList[i].temperature;
        int N = paramsList[i].size * paramsList[i].size;  // Assuming 2D lattice
        double chi = susceptibility(customAverages[i], beta, N);
        double Cv = specificHeat(customAverages[i], beta, N);
        double avgE = averageEnergy(customAverages[i]);
        std::cout << "T=" << paramsList[i].temperature << ", Chi=" << chi << ", Cv=" << Cv << ", E=" << avgE << "\n";
    }

    outFile.close();
    tempFile.close();

    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = endTime - startTime;
    std::cout << "Simulation completed in " << elapsed.count() << " seconds.\n";

    return 0;

}