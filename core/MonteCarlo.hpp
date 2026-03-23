#ifndef MONTECARLO_HPP
#define MONTECARLO_HPP

#include "Lattices.hpp"
#include "Lattices1D.hpp"
#include "Lattices2D.hpp"
#include "Lattices3D.hpp"

#include <vector>
#include <iostream>
#include <random>
#include <cmath>
#include <thread>
#include <mutex>
#include <memory>
#include <deque>
#include <fftw3.h> //Fourierificering, comment ud indtil i installerer library

enum stepType
{
    Metropolis,
    Wolff
};

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
    double stabilizing = 0.2;
    stepType stepAlgorithm = stepType::Metropolis;
    int wolffStabilizationSteps = 0; // Number of Wolff steps for stabilization if wolffStabilization is true
    
};

struct MCFourierResult
{
    std::vector<double> normKvadrat = std::vector<double>();
    std::vector<double> normKvadratVarians = std::vector<double>();

    double magnetisering;
    double magnetiseringVarians;
    long double magnetiseringVariansVarians;

    double hamilton;
    double hamiltonVarians;
    long double hamiltonVariansVarians;

    int count;
};

void MCStepMetropolis(Lattice &lattice, double T, std::mt19937 &rng, std::uniform_real_distribution<double> &distReal)
{
    int p = lattice.getRandomLatticeIndex();
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

void MCStepWolff(Lattice &lattice, double T, std::mt19937 &rng, std::uniform_real_distribution<double> &distReal)
{
    int p = lattice.getRandomLatticeIndex();
    int originalSpin = lattice.getSpin(p);
    std::vector<int> cluster;
    std::vector<bool> visited(lattice.getSpinsSize(), false);

    std::vector<int> stack;
    stack.push_back(p);
    visited[p] = true;

    while (!stack.empty())
    {
        int currentIndex = stack.back();
        stack.pop_back();
        cluster.push_back(currentIndex);

        for (const Interaction &interaction : lattice.getInteractions(currentIndex))
        {
            int neighborIndex = interaction.neighbor;
            if (!visited[neighborIndex] && lattice.getSpin(neighborIndex) == originalSpin)
            {
                double J = interaction.J;
                double P_add = 1 - std::exp(-2 * J / T); //magic formula

                if (distReal(rng) < P_add)
                {
                    visited[neighborIndex] = true;
                    stack.push_back(neighborIndex);
                }
            }
        }
    }

    for (int index : cluster)
    {
        lattice.flipSpin(index);
    }

    lattice.stepForward();
}

void MCStep(Lattice &lattice, double T, std::mt19937 &rng, std::uniform_real_distribution<double> &distReal, stepType algorithm)
{
    if (algorithm == stepType::Wolff)
    {
        MCStepWolff(lattice, T, rng, distReal);
    }
    else if (algorithm == stepType::Metropolis)
    {
        MCStepMetropolis(lattice, T, rng, distReal);
    }
    else
    {
        std::cerr << "Unsupported step algorithm!" << std::endl;
    }
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

    Lattice *lattice;

    switch (params.latticeType)
    {
    case LatticeType::Square:
        lattice = new SquareLattice2D(params.size, params.B);
        break;
    case LatticeType::FunkySquare:
        lattice = new FunkySquareLattice2D(params.size, params.B);
        break;
    // case LatticeType::Cubic:
    //     lattice = new Cubic(params.size, params.B);
    //     break;
    case LatticeType::OneD:
        lattice = new FreeLattice1D(params.size, params.B);
        break;
    case LatticeType::Triangle:
        lattice = new TriangleLattice2D(params.size, params.B);
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
        MCStep(*lattice, params.temperature, rng, distReal, params.stepAlgorithm);
        if (i % params.measurementInterval == 0)
        {
            measurements.push_back(lattice->measure());
            if (params.printProgress)
            {
                std::lock_guard<std::mutex> lock(mcPrintMutex());
                //lattice->printLarge(0, lattice->getSize(), lattice->getSize() / 20); // doesn't work in n dimensions
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

MCFourierResult runFourierMCSimulation(const MCParameters &params)
{
    std::mt19937 rng{std::random_device{}()};
    std::uniform_real_distribution<double> distReal{0.0, 1.0};
    MCFourierResult output;

    Lattice *lattice;

    switch (params.latticeType)
    {
    // case LatticeType::Square:
    //     lattice = new SquareLattice2D(params.size, params.B);
    //     break;
    case LatticeType::FunkySquare:
        lattice = new FunkySquareLattice2D(params.size, params.B);
        break;
    // case LatticeType::Cubic:
    //     lattice = new Cubic(params.size, params.B);
    //     break;
    case LatticeType::OneD:
        lattice = new FreeLattice1D(params.size, params.B);
        break;
    case LatticeType::Triangle:
        lattice = new TriangleLattice2D(params.size, params.B);
        break;
    default:
        std::cerr << "Unsupported lattice type!" << std::endl;
        return output;
    }

    lattice->initializeFourier();

    if (params.randomize)
    {
        lattice->randomize();
    }

    for (long int i = 0; i < params.totalStepCount; i++)
    {
        MCStep(*lattice, params.temperature, rng, distReal, params.stepAlgorithm);
        if (i % params.measurementInterval == 0 && double(i)/params.totalStepCount > params.stabilizing)
        {
            lattice->updateMeasurementTracker();
        }
        else if (params.wolffStabilizationSteps > 0 && i % params.measurementInterval == 0 ) {
            for (int j = 0; j < params.wolffStabilizationSteps; j++) {
                MCStepWolff(*lattice, params.temperature, rng, distReal);
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
        std::cout << "\n";
        std::cout << "Simulation complete for T = " << params.temperature << ", B = " << params.B << ", size = " << params.size << "\n";
    }

    TrackMeasurements measurements = lattice->getMeasurements();
    for (size_t i = 0; i < measurements.normKvadratSum.size(); ++i)
    {
        output.normKvadrat.push_back(measurements.normKvadratSum[i] / measurements.counter);
        output.normKvadratVarians.push_back(
            (measurements.normKvadratKvadratSum[i] / measurements.counter) 
            - (measurements.normKvadratSum[i] / measurements.counter) * (measurements.normKvadratSum[i] / measurements.counter)
        ); // Varians af normKvadrat, normaliseret
    }

    output.count = measurements.counter;

    output.magnetisering = measurements.magnetiseringSum / measurements.counter;
    output.magnetiseringVarians = ((measurements.magnetiseringKvadratSum / measurements.counter) - (measurements.magnetiseringSum / measurements.counter) * (measurements.magnetiseringSum / measurements.counter)) * (float(measurements.counter) / (float(measurements.counter) - 1)); // Varians af magnetisering, normaliseret

    output.hamilton = measurements.hamiltonSum / measurements.counter;
    output.hamiltonVarians = ((measurements.hamiltonKvadratSum / measurements.counter) - (measurements.hamiltonSum / measurements.counter) * (measurements.hamiltonSum / measurements.counter)) * (float(measurements.counter) / (float(measurements.counter) - 1)); // Varians af hamilton, normaliseret

    
    float magnetiseringMu4 = (measurements.magnetiseringKvadratKvadratSum / measurements.counter) 
                - 4 * (measurements.magnetiseringSum / measurements.counter) * (measurements.magnetiseringKubeSum / measurements.counter)
                + 6 * (measurements.magnetiseringSum / measurements.counter) * (measurements.magnetiseringSum / measurements.counter) * (measurements.magnetiseringKvadratSum / measurements.counter)
                - 3 * (measurements.magnetiseringSum / measurements.counter) * (measurements.magnetiseringSum / measurements.counter) * (measurements.magnetiseringSum / measurements.counter) * (measurements.magnetiseringSum / measurements.counter);
    
    float hamiltonMu4 = (measurements.hamiltonKvadratKvadratSum / measurements.counter) 
                - 4 * (measurements.hamiltonSum / measurements.counter) * (measurements.hamiltonKubeSum / measurements.counter)
                + 6 * (measurements.hamiltonSum / measurements.counter) * (measurements.hamiltonSum / measurements.counter) * (measurements.hamiltonKvadratSum / measurements.counter)
                - 3 * (measurements.hamiltonSum / measurements.counter) * (measurements.hamiltonSum / measurements.counter) * (measurements.hamiltonSum / measurements.counter) * (measurements.hamiltonSum / measurements.counter);

    output.magnetiseringVariansVarians = (magnetiseringMu4 - (float(measurements.counter) - 3)/ (float(measurements.counter) - 1) * output.magnetiseringVarians * output.magnetiseringVarians);
    output.hamiltonVariansVarians = (hamiltonMu4 - (float(measurements.counter) - 3)/ (float(measurements.counter) - 1) * output.hamiltonVarians * output.hamiltonVarians);

    std::cout << "summer over magnetisering, varians og variansvarians, " << measurements.magnetiseringSum << ", " << measurements.magnetiseringKvadratSum << ", " << measurements.magnetiseringKvadratKvadratSum << "\n"; 
    std::cout << "summer over hamilton, varians og variansvarians, " << measurements.hamiltonSum << ", " << measurements.hamiltonKvadratSum << ", " << measurements.hamiltonKvadratKvadratSum << "\n";

    // FIX DET ER SKRAMMEL DET VIRKER IK!!!!!!!!!

    lattice->freeFourier();
    delete lattice;
    return output;
}
void writeFourierMCResultsToArray(std::vector<MCFourierResult> &allMeasurements, const MCParameters &params, int index)
{
    allMeasurements[index] = runFourierMCSimulation(params);
}

std::vector<MCFourierResult> runParallelFourierMCSimulation(std::vector<MCParameters> &paramsList)
{
    std::vector<MCFourierResult> allMeasurements(paramsList.size());

    std::deque<std::thread> threads;
    unsigned int hw = std::thread::hardware_concurrency();
    size_t maxThreads = (hw == 0) ? 4 : static_cast<size_t>(hw);

    for (size_t i = 0; i < paramsList.size(); ++i)
    {
        if (threads.size() >= maxThreads)
        {
            threads.front().join();
            threads.pop_front();
        }

        paramsList[i].printProgress = false; // Disable progress printing for parallel runs
        threads.emplace_back(writeFourierMCResultsToArray, std::ref(allMeasurements), std::cref(paramsList[i]), i);

    }

    for (auto &thread : threads)
    {
        thread.join();
    }

    return allMeasurements;
}








#endif // MONTECARLO_HPP