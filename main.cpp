#include "MonteCarlo2D.hpp"
#include "lattices2D.hpp"

#include <vector>
#include <iostream>
#include <random>

int main()
{
    int stepCount = 50000000;
    int latticeSize = 1000;
    double temperature = 1;
    std::mt19937 rng{std::random_device{}()};
    std::uniform_real_distribution<double> distReal{0.0, 1.0};

    SquareLattice2D lattice(latticeSize);
    lattice.randomize();
    lattice.printLarge(0, 30, 1);

    for (int i = 0; i < stepCount; i++)
    {
        MCStep2D(lattice, temperature, rng, distReal);
        if (i % 9900000 == 0)
        {
            std::cout << "Step " << i << ":\n";
            lattice.printLarge(0, 200, 7);
        }
    }

    return 0;
}