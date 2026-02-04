#include "MonteCarlo2D.hpp"
#include "lattices2D.hpp"

#include <vector>
#include <iostream>
#include <random>

int main() {
    int latticeSize = 30;
    double temperature = 100;
    std::mt19937 rng{std::random_device{}()};
    std::uniform_real_distribution<double> distReal{0.0, 1.0};
    
    
    SquareLattice2D lattice(latticeSize);
    lattice.print();
    MCStep2D(lattice,temperature, rng, distReal);
    lattice.print();
    lattice.randomize();
    lattice.print();
    


    return 0;
}