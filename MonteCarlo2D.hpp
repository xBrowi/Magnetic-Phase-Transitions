#ifndef MONTECARLO_HPP
#define MONTECARLO_HPP

#include "lattices2D.hpp"

#include <vector>
#include <iostream>
#include <random>
#include <cmath>

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
}

#endif // MONTECARLO_HPP