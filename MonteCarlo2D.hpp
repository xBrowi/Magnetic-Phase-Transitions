#ifndef MONTECARLO_HPP
#define MONTECARLO_HPP

#include "lattices2D.hpp"

#include <vector>
#include <iostream>
#include <random>
#include <cmath>
#include <thread>


struct MCParameters
{
    LatticeType2D latticeType;
    int size;
    double temperature;
    int totalStepCount;
    int measurementInterval;
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

std::vector<measurement2D> runMCSimulation(const MCParameters& params)
{
    std::vector<measurement2D> measurements;
    std::mt19937 rng{std::random_device{}()};
    std::uniform_real_distribution<double> distReal{0.0, 1.0};

    Lattice2D *lattice;

    switch (params.latticeType)
    {
    case LatticeType2D::Square:
        lattice = new SquareLattice2D(params.size);
        break;
    case LatticeType2D::FunkySquare:
        lattice = new FunkySquareLattice2D(params.size);
        break;
    default:
        std::cerr << "Unsupported lattice type!" << std::endl;
        return measurements;
    }

    if (params.randomize) {
        lattice->randomize();
    }

    for (int i = 0; i < params.totalStepCount; i++)
    {
        MCStep2D(*lattice, params.temperature, rng, distReal);
        if (i % params.measurementInterval == 0)
        {
            measurements.push_back(lattice->measure());
            if (params.printProgress) {
                lattice->printLarge(0, lattice->getSize(), lattice->getSize() / 20);
                std::cout << "Step: " << i << ", Magnetization: " << lattice->magnetization() << "\n";
            }
        }

        if (i % (params.totalStepCount / 50) == 0)
        {
            std::cout << "Progress: " << (100.0 * i / params.totalStepCount) << "%\n";
        }
    }

    delete lattice;
    return measurements;
}

void writeMCResultsToArray(std::vector<std::vector<measurement2D>>& allMeasurements, const MCParameters& params, int index)
{
    allMeasurements[index] = runMCSimulation(params);
}

std::vector<std::vector<measurement2D>> runParallelMCSimulation(std::vector<MCParameters>& paramsList)
{
    std::vector<std::vector<measurement2D>> allMeasurements(paramsList.size());

    std::vector<std::thread> threads;
    for (size_t i = 0; i < paramsList.size(); ++i)
    {
        paramsList[i].printProgress = false; // Disable progress printing for parallel runs
        threads.emplace_back(writeMCResultsToArray, std::ref(allMeasurements), std::cref(paramsList[i]), i);
    }

    for (auto& thread : threads) {
        thread.join();
    }

    return allMeasurements;
}

#endif // MONTECARLO_HPP