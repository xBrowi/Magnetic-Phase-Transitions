#ifndef MONTECARLO_HPP
#define MONTECARLO_HPP

#include "Lattices.hpp"

#include <vector>
#include <iostream>
#include <random>
#include <cmath>
#include <thread>
#include <mutex>

struct MCParameters
{
    LatticeType latticeType;
    int size;
    double temperature;
    double B;
    long int totalStepCount;
    long int measurementInterval;
    bool randomize = true;
    bool printProgress = false;
};

void MCStep2D(Lattice2D &lattice, double T, std::mt19937 &rng, std::uniform_real_distribution<double> &distReal)
{
    Point2D p = lattice.getRandomCoord();
    double dH = lattice.deltaH(p);

    if (dH < 0)
    {
        lattice.flipSpin(p);
    }
    else
    {
        if (distReal(rng) < std::exp(-dH / (T)))
        { // antag kB = 1, da J normaliseret
            lattice.flipSpin(p);
        }
    }

    lattice.stepForward();
}

inline std::mutex &mcPrintMutex()
{
    static std::mutex mtx;
    return mtx;
}

std::vector<Measurement> runMCSimulation(const MCParameters &params)
{
    std::vector<Measurement> measurements;
    std::mt19937 rng{std::random_device{}()};
    std::uniform_real_distribution<double> distReal{0.0, 1.0};

    Lattice2D *lattice;

    switch (params.latticeType)
    {
    case LatticeType::Square:
        lattice = new SquareLattice2D(params.size, params.B);
        break;
    case LatticeType::FunkySquare:
        lattice = new FunkySquareLattice2D(params.size, params.B);
        break;
    default:
        std::cerr << "Unsupported lattice type!" << std::endl;
        return measurements;
    }

    if (params.randomize)
    {
        lattice->randomize();
    }

    for (long int i = 0; i < params.totalStepCount; i++)
    {
        MCStep2D(*lattice, params.temperature, rng, distReal);
        if (i % params.measurementInterval == 0)
        {
            measurements.push_back(lattice->measure());
            if (params.printProgress)
            {
                std::lock_guard<std::mutex> lock(mcPrintMutex());
                lattice->printLarge(0, lattice->getSize(), lattice->getSize() / 20);
                std::cout << "Step: " << i << ", Magnetization: " << lattice->magnetization() << "\n";
            }
        }

        if (i % (params.totalStepCount / 50) == 0)
        {
            std::lock_guard<std::mutex> lock(mcPrintMutex());
            std::cout << "Progress: " << (100.0 * i / params.totalStepCount) << "%\n";
        }
    }

    {
        std::lock_guard<std::mutex> lock(mcPrintMutex());
        std::cout << "Simulation complete for T = " << params.temperature << ", B = " << params.B << ", size = " << params.size << "\n";
    }

    delete lattice;
    return measurements;
}

void writeMCResultsToArray(std::vector<std::vector<Measurement>> &allMeasurements, const MCParameters &params, int index)
{
    allMeasurements[index] = runMCSimulation(params);
}

std::vector<std::vector<Measurement>> runParallelMCSimulation(std::vector<MCParameters> &paramsList)
{
    std::vector<std::vector<Measurement>> allMeasurements(paramsList.size());

    std::vector<std::thread> threads;
    for (size_t i = 0; i < paramsList.size(); ++i)
    {
        paramsList[i].printProgress = false; // Disable progress printing for parallel runs
        threads.emplace_back(writeMCResultsToArray, std::ref(allMeasurements), std::cref(paramsList[i]), i);
    }

    for (auto &thread : threads)
    {
        thread.join();
    }

    return allMeasurements;
}

#endif // MONTECARLO_HPP