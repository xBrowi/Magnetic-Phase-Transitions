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
    params.size = 10;
    params.temperature = 2;
    params.B = 0;
    params.totalStepCount = 1e3;
    params.measurementInterval = 1e2;
    params.randomize = true;
    params.printProgress = false;

    std::mt19937 rng{std::random_device{}()};
    std::uniform_real_distribution<double> distReal{0.0, 1.0};

    Lattice2D *lattice;
    lattice = new TriangleLattice2D(params.size, params.B);

    
    lattice->randomize();
    lattice->print();

    for (int i = 0;i<10;i++)
    {
        MCStep(*lattice,params.temperature,rng,distReal);
    }

    lattice->print();

    int p = lattice->getRandomLatticeIndex();
    std::vector<Interaction> interactions = lattice->getInteractions(p);

    Point2D pCoord = lattice->getCoordFromIndex(p);
    std::cout << "current point: " << pCoord.x << "," << pCoord.y << std::endl;
    std::cout << "neighboring points: "; 
    for (Interaction i : interactions) {
        Point2D neighborCoord = lattice->getCoordFromIndex(i.neighbor);
        std::cout << neighborCoord.x << "," << neighborCoord.y << " ";
    }
    std::cout << std::endl;


    return 0;
}