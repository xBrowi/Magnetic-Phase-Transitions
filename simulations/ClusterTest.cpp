
#include "../core/ClusterAnalysis.hpp"
#include "../core/MonteCarlo.hpp"
#include "../core/Lattices.hpp"

#include <vector>
#include <iostream>
#include <fstream>
#include <random>
#include <chrono>




int main() {

    MCParameters params;
    params.latticeType = LatticeType::FunkySquare;
    params.size = 20;
    params.temperature = 2.05;
    params.totalStepCount = 1e8;
    params.measurementInterval = 10000;
    params.randomize = true;
    params.printProgress = false;

    Lattice2D *lattice;
    lattice = new FunkySquareLattice2D(params.size);

    lattice->randomize();

    lattice->print();

    std::vector<int> clusterSizes = ClusterSizes(*lattice);

    std::cout << "\n Clustersize 0: " << clusterSizes[0] << "\n";

    return 0;
}