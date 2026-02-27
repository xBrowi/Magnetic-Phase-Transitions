#ifndef LATTICES2D_HPP
#define LATTICES2D_HPP

#include "Lattices.hpp"

// This is a 2D point class. The function print() prints its coordinates.
struct Point2D
{
    int x, y;
};

// an interaction class, storing the neighbors coordinates and the coupling strength
struct interaction2D
{
    Point2D neighbor;
    double J; // OBS: IN UNITS OF K_B (J/K_B)
};

// Template class for lattices. All other lattice classes should inherit these properties.
class Lattice2D : public Lattice
{
    // private is for stuff that should not be accessed, changed or deleted outside the 'definition' of the class.
protected:
    // Jeg tror, man normalt kalder spins for spinConfig. Værd at overveje navneskift


public:
    // Constructor: initialisér et gitter med alle spins opad (+1)
    Lattice2D(int sizeArg, double argB = 0)
    {
        size = sizeArg;
        B = argB;
        step = 0;
        spins = std::vector<int>(sizeArg * sizeArg, 1);
        distIndex = std::uniform_int_distribution<int>(0, static_cast<int>(spins.size() - 1));
    }

    int getSpin(Point2D p)
    {
        return spins[p.x * size + p.y];
    }

    Point2D getCoordFromIndex(int index)
    {
        return Point2D{index / size, index % size};
    }


    // Flip spin ved et givent koordinat
    void flipSpin(Point2D p)
    {
        spins[p.x * size + p.y] *= -1;
    }

    // Vælger et tilfældigt koordinat
    Point2D getRandomCoord()
    {
        return getCoordFromIndex(getRandomLatticeIndex());
    }

    void print()
    {
        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < size; j++)
            {
                int spin = getSpin({j, i});
                std::cout << (spin == 1 ? "██" : "░░");
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }

    // Funktion til at printe et udsnit af gitteret med en given step for at undgå at printe hele gitteret, hvis det er stort. - Alvin 5.2.2026
    void printLarge(int lower = 0, int upper = 0, int step = 1)
    {
        if (upper == 0)
            upper = size;

        for (int m = lower; m < upper; m += step)
        {
            for (int n = lower; n < upper; n += step)
            {
                std::cout << (getSpin({m, n}) == 1 ? "██" : "░░");
            }
            std::cout << "\n";
        }
    }

    virtual std::vector<interaction2D> getInteractions2D(Point2D p) = 0;
    virtual double deltaH(Point2D p) = 0;

    double deltaH(int index) override
    {
        Point2D p = getCoordFromIndex(index);
        return deltaH(p);
    }

    std::vector<Interaction> getInteractions(int index) override
    {
        Point2D p = getCoordFromIndex(index);
        std::vector<interaction2D> interactions2D = getInteractions2D(p);
        std::vector<Interaction> interactions;

        for (const interaction2D &interaction : interactions2D)
        {
            int neighborIndex = interaction.neighbor.x * size + interaction.neighbor.y;
            interactions.push_back(Interaction{neighborIndex, interaction.J});
        }

        return interactions;
    }
};

// square structure with normal periodic boundary conditions
class SquareLattice2D : public Lattice2D
{
public:
    SquareLattice2D(int sizeArg, double BArg = 0) : Lattice2D(sizeArg, BArg) {}

    std::vector<interaction2D> getInteractions2D(Point2D p) override
    {
        std::vector<interaction2D> interactions;
        double J = 1; // Interaction strength in units of k_B (J/k_B)

        interactions.push_back(interaction2D{Point2D{(p.x + 1) % size, p.y}, J});
        interactions.push_back(interaction2D{Point2D{(p.x - 1) % size, p.y}, J});
        interactions.push_back(interaction2D{Point2D{p.x, (p.y + 1) % size}, J});
        interactions.push_back(interaction2D{Point2D{p.x, (p.y - 1) % size}, J});

        return interactions;
    }

    double deltaH(Point2D p) override
    {
        double H = -B; // B is magnetic moment times magnetic field

        for (const interaction2D &interaction : getInteractions2D(p))
        {
            H -= getSpin(interaction.neighbor) * interaction.J; // Her mangler p's eget spin, som skal indgå i beregningen af dH
        }

        double dH = -2 * getSpin(p) * H;

        return dH;
    }
    
};

// square structure with warped periodic boundary conditions
class FunkySquareLattice2D : public Lattice2D
{
public:
    FunkySquareLattice2D(int sizeArg, double BArg = 0) : Lattice2D(sizeArg, BArg) {}

    std::vector<interaction2D> getInteractions2D(Point2D p) override
    {
        std::vector<interaction2D> interactions;
        double J = 1; // Interaction strength in units of k_B (J/k_B)

        if (p.x > 0)
            interactions.push_back(interaction2D{Point2D{p.x - 1, p.y}, J});
        else if (p.x == 0)
            interactions.push_back(interaction2D{Point2D{size - 1 - p.y, size - 1}, J}); // Funky boundary

        if (p.x < size - 1)
            interactions.push_back(interaction2D{Point2D{p.x + 1, p.y}, J});
        else if (p.x == size - 1)
            interactions.push_back(interaction2D{Point2D{size - 1 - p.y, 0}, J}); // Funky boundary

        if (p.y > 0)
            interactions.push_back(interaction2D{Point2D{p.x, p.y - 1}, J});
        else if (p.y == 0)
            interactions.push_back(interaction2D{Point2D{size - 1, size - 1 - p.x}, J}); // Funky boundary

        if (p.y < size - 1)
            interactions.push_back(interaction2D{Point2D{p.x, p.y + 1}, J});
        else if (p.y == size - 1)
            interactions.push_back(interaction2D{Point2D{0, size - 1 - p.x}, J}); // Funky boundary

        return interactions;
    }

    double deltaH(Point2D p) override
    {
        double H = -B;

        for (const interaction2D &interaction : getInteractions2D(p))
        {
            H -= getSpin(interaction.neighbor) * interaction.J; // Her mangler p's eget spin, som skal indgå i beregningen af dH
        }

        double dH = -2 * getSpin(p) * H;

        return dH;
    }
};

// triangular structure with normal periodic boundary conditions (J defaults -1, antiferromagnetic)
class TriangleLattice2D : public Lattice2D
{
public:
    TriangleLattice2D(int sizeArg, double BArg = 0) : Lattice2D(sizeArg, BArg) {}

    std::vector<interaction2D> getInteractions2D(Point2D p) override
    {
        std::vector<interaction2D> interactions;
        double J = -1; // Interaction strength in units of k_B (J/k_B)

        if (p.x % 2 == 1) //lige rækker er rykket ind mod højre ift. square
        {
            interactions.push_back(interaction2D{Point2D{(p.x - 1) % size, (p.y) % size}, J}); //venste
            interactions.push_back(interaction2D{Point2D{(p.x + 1) % size, (p.y) % size}, J}); //højre
            interactions.push_back(interaction2D{Point2D{(p.x - 1) % size, (p.y + 1) % size}, J}); //nordvest
            interactions.push_back(interaction2D{Point2D{(p.x - 1) % size, (p.y - 1) % size}, J}); //sydvest
            interactions.push_back(interaction2D{Point2D{(p.x) % size, (p.y + 1) % size}, J}); //nordøst
            interactions.push_back(interaction2D{Point2D{(p.x) % size, (p.y - 1) % size}, J}); //sydøst
        }
        else //ulige rækker 
        {
            interactions.push_back(interaction2D{Point2D{(p.x - 1) % size, (p.y) % size}, J}); //venste
            interactions.push_back(interaction2D{Point2D{(p.x + 1) % size, (p.y) % size}, J}); //højre
            interactions.push_back(interaction2D{Point2D{(p.x) % size, (p.y + 1) % size}, J}); //nordvest
            interactions.push_back(interaction2D{Point2D{(p.x) % size, (p.y - 1) % size}, J}); //sydvest
            interactions.push_back(interaction2D{Point2D{(p.x + 1) % size, (p.y + 1) % size}, J}); //nordøst
            interactions.push_back(interaction2D{Point2D{(p.x + 1) % size, (p.y - 1) % size}, J}); //sydøst
        }
        return interactions;
    }

    double deltaH(Point2D p) override
    {
        double H = -B;

        for (const interaction2D &interaction : getInteractions2D(p))
        {
            H -= getSpin(interaction.neighbor) * interaction.J; // Her er p's eget spin faktoriseret ud, se i beregningen af dH
        }

        double dH = -2 * getSpin(p) * H;

        return dH;
    }
};

#endif // LATTICES2D_HPP