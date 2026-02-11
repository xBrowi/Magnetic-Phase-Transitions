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
        MCStep2D(*lattice,params.temperature,rng,distReal);
    }

    lattice->print();

    Point2D p = lattice->getRandomCoord();
    std::vector<interaction2D> interactions = lattice->getInteractions(p);

    std::cout << "current point: " << p.x << "," << p.y << std::endl;
    std::cout << "neighboring points: "; 
    for (interaction2D i : interactions) {
        std::cout << i.neighbor.x << "," << i.neighbor.y << " ";
    }
    std::cout << std::endl;


    return 0;
}