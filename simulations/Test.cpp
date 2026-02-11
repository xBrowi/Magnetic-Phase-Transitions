#include "../core/ClusterAnalysis.hpp"
#include "../core/MonteCarlo2D.hpp"
#include "../core/Lattices2D.hpp"

#include <vector>
#include <iostream>
#include <fstream>
#include <random>
#include <chrono>




int main() {
    MCParameters params;
    params.latticeType = LatticeType2D::FunkySquare;
    params.size = 10;
    params.temperature = 2;
    params.B = -50;
    params.totalStepCount = 1e3;
    params.measurementInterval = 1e2;
    params.randomize = true;
    params.printProgress = false;

    std::mt19937 rng{std::random_device{}()};
    std::uniform_real_distribution<double> distReal{0.0, 1.0};

    Lattice2D *lattice;
    lattice = new FunkySquareLattice2D(params.size, params.B);

    
    lattice->randomize();


    lattice->print();

    double meanSizebefore = meanClusterSize(ClusterSizes(*lattice));

    std::cout << "Mean cluster size: " << meanSizebefore << "\n";
    
    for (int i = 0;i<1e4;i++)
    {
        MCStep2D(*lattice,params.temperature,rng,distReal);
    }

    lattice->print();

    double meanSize = meanClusterSize(ClusterSizes(*lattice));

    std::cout << "Mean cluster size: " << meanSize << "\n";

    return 0;
}