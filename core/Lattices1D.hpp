#ifndef LATTICES1D_HPP
#define LATTICES1D_HPP

#include "Lattices.hpp"

struct Point1D
{
    int x;
};

// an interaction class, storing the neighbors coordinates and the coupling strength
struct interaction1D
{
    Point1D neighbor;
    double J; // OBS: IN UNITS OF K_B (J/K_B)
};

class Lattice1D : public Lattice
{

public:
    // Constructor: initialisér et gitter med alle spins opad (+1)
    Lattice1D(int sizeArg, double argB = 0)
    {
        size = sizeArg;
        B = argB;
        step = 0;
        spins = std::vector<int>(sizeArg, 1);
        distIndex = std::uniform_int_distribution<int>(0, static_cast<int>(spins.size() - 1));
    }

    int getSpin(Point1D p)
    {
        return spins[p.x];
    }

    void flipSpin(Point1D p)
    {
        spins[p.x] *= -1;
    }

    Point1D getCoordFromIndex(int index)
    {
        return Point1D{index};
    }

    // Vælger et tilfældigt koordinat
    Point1D getRandomCoord()
    {
        return getCoordFromIndex(getRandomLatticeIndex());
    }

    void print()
    {
        for (int i = 0; i < size; i++)
        {
            int spin = getSpin(getCoordFromIndex(i));
            std::cout << (spin == 1 ? "██" : "░░");
        }
        std::cout << "\n";
    }

    virtual std::vector<interaction1D> getInteractions1D(Point1D p) = 0;
    virtual double deltaH(Point1D p) = 0;

    double deltaH(int index) override
    {
        Point1D p = getCoordFromIndex(index);
        return deltaH(p);
    }

    std::vector<Interaction> getInteractions(int index) override
    {
        Point1D p = getCoordFromIndex(index);
        std::vector<interaction1D> interactions1D = getInteractions1D(p);
        std::vector<Interaction> interactions;

        for (const interaction1D &interaction : interactions1D)
        {
            int neighborIndex = interaction.neighbor.x;
            interactions.push_back(Interaction{neighborIndex, interaction.J});
        }

        return interactions;
    }

};

// 3 * 1 lattice with free boundary conditions
class FreeLattice1D : public Lattice1D
{
public:
    FreeLattice1D(int sizeArg, double BArg = 0) : Lattice1D(sizeArg, BArg) {}

    std::vector<interaction1D> getInteractions1D(Point1D p) override
    {
        std::vector<interaction1D> interactions;
        double J = 1; // Interaction strength in units of k_B (J/k_B)

        if (p.x > 0)
            interactions.push_back(interaction1D{Point1D{p.x - 1}, J});
        if (p.x < size - 1)
            interactions.push_back(interaction1D{Point1D{p.x + 1}, J});

        return interactions;
    }

    double deltaH(Point1D p) override
    {
        double H = -B; // B is magnetic moment times magnetic field

        for (const interaction1D &interaction : getInteractions1D(p))
        {
            H -= getSpin(interaction.neighbor) * interaction.J; // Her mangler p's eget spin, som skal indgå i beregningen af dH
        }

        double dH = -2 * getSpin(p) * H;

        return dH;
    }
};

#endif // LATTICES1D_HPP