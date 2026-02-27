#ifndef LATTICES3D_HPP
#define LATTICES3D_HPP

#include "Lattices.hpp"

// This is a 3D point class. The function print() prints its coordinates.
struct Point3D
{
    int x, y, z;
};

// an interaction class, storing the neighbors coordinates and the coupling strength
struct interaction3D
{
    Point3D neighbor;
    double J; // OBS: IN UNITS OF K_B (J/K_B)
};

// Template class for lattices. All other lattice classes should inherit these properties.
class Lattice3D : public Lattice
{
    // private is for stuff that should not be accessed, changed or deleted outside the 'definition' of the class.
protected:
    // Jeg tror, man normalt kalder spins for spinConfig. Værd at overveje navneskift


public:
    // Constructor: initialisér et gitter med alle spins opad (+1)
    Lattice3D(int sizeArg, double argB = 0)
    {
        size = sizeArg;
        B = argB;
        step = 0;
        spins = std::vector<int>(sizeArg * sizeArg * sizeArg, 1);
        distIndex = std::uniform_int_distribution<int>(0, static_cast<int>(spins.size() - 1));
    }

    int getSpin(Point3D p)
    {
        return spins[p.x * size * size + p.y * size + p.z];
    }

    Point3D getCoordFromIndex(int index)
    {
        return Point3D{index / (size * size), (index % (size * size)) / size, index % size};
    }


    // Flip spin ved et givent koordinat
    void flipSpin(Point3D p)
    {
        spins[p.x * size * size + p.y * size + p.z] *= -1;
    }

    // Vælger et tilfældigt koordinat
    Point3D getRandomCoord()
    {
        return getCoordFromIndex(getRandomLatticeIndex());
    }

    void print()
    {
        for (int m = 0; m < size; m++)
        {
            for (int n = 0; n < size; n++)
            {
                for (int o = 0; o < size; o++)
                {
                    std::cout << (getSpin({m, n, o}) == 1 ? "██" : "░░");
                }
                std::cout << "\n";
            }
            std::cout << "\n\n";
        }
    }

    virtual std::vector<interaction3D> getInteractions3D(Point3D p) = 0;
    virtual double deltaH(Point3D p) = 0;

    double deltaH(int index) override
    {
        Point3D p = getCoordFromIndex(index);
        return deltaH(p);
    }

    std::vector<Interaction> getInteractions(int index) override
    {
        Point3D p = getCoordFromIndex(index);
        std::vector<interaction3D> interactions3D = getInteractions3D(p);
        std::vector<Interaction> interactions;

        for (const interaction3D &interaction : interactions3D)
        {
            int neighborIndex = interaction.neighbor.x * size * size + interaction.neighbor.y * size + interaction.neighbor.z;
            interactions.push_back(Interaction{neighborIndex, interaction.J});
        }

        return interactions;
    }
};

// square structure with normal periodic boundary conditions
class Cubic : public Lattice3D
{
public:
    Cubic(int sizeArg, double BArg = 0) : Lattice3D(sizeArg, BArg) {}

    std::vector<interaction3D> getInteractions3D(Point3D p) override
    {
        std::vector<interaction3D> interactions;
        double J = 1; // Interaction strength in units of k_B (J/k_B)

        if (p.x < size - 1)
            interactions.push_back(interaction3D{Point3D{p.x + 1, p.y, p.z}, J});

        if (p.x > 0)
            interactions.push_back(interaction3D{Point3D{p.x - 1, p.y, p.z}, J});

        if (p.y < size - 1)
            interactions.push_back(interaction3D{Point3D{p.x, p.y + 1, p.z}, J});

        if (p.y > 0)
            interactions.push_back(interaction3D{Point3D{p.x, p.y - 1, p.z}, J});

        if (p.z < size - 1)
            interactions.push_back(interaction3D{Point3D{p.x, p.y, p.z + 1}, J});

        if (p.z > 0)
            interactions.push_back(interaction3D{Point3D{p.x, p.y, p.z - 1}, J});

        return interactions;
    }

    double deltaH(Point3D p) override
    {
        double H = -B; // B is magnetic moment times magnetic field

        for (const interaction3D &interaction : getInteractions3D(p))
        {
            H -= getSpin(interaction.neighbor) * interaction.J; // Her mangler p's eget spin, som skal indgå i beregningen af dH
        }

        double dH = -2 * getSpin(p) * H;

        return dH;
    }
    
};

#endif // LATTICES3D_HPP