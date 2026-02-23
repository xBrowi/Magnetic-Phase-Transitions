#ifndef LATTICES2D_HPP
#define LATTICES2D_HPP

#include <vector>
#include <iostream>
#include <random>

// This is a 2D point class. The function print() prints its coordinates.
struct Point2D
{
    int x, y;

    void print()
    {
        std::cout << "Point2D(" << x << ", " << y << ")\n";
    }
};

// A measurement class for storing the results of MC simulations at a timestep.
struct measurement2D
{
    long int step;
    double magnetization;
    double meanClusterSize;
};

// identifier for various 2D lattice types
enum class LatticeType2D
{
    Square,
    FunkySquare
};

// an interaction class, storing the neighbors coordinates and the coupling strength
struct interaction2D
{
    Point2D neighbor;
    double J; // OBS: IN UNITS OF K_B (J/K_B)
};

// Template class for lattices. All other lattice classes should inherit these properties.
class Lattice2D
{
    // private is for stuff that should not be accessed, changed or deleted outside the 'definition' of the class.
protected:
    // Jeg tror, man normalt kalder spins for spinConfig. Værd at overveje navneskift
    int size;
    long int step;
    std::vector<std::vector<int>> spins;
    double B;

    // Random number generator (rng)
    std::mt19937 rng{std::random_device{}()};
    std::uniform_int_distribution<int> distCoord;
    std::uniform_real_distribution<double> distReal{0.0, 1.0};

public:
    // Constructor: initialisér et gitter med alle spins opad (+1)
    Lattice2D(int sizeArg, double argB = 0)
    {
        size = sizeArg;
        B = argB;
        step = 0;
        spins = std::vector<std::vector<int>>(sizeArg, std::vector<int>(sizeArg, 1));
        distCoord = std::uniform_int_distribution<int>(0, size - 1);
    }

    // Muliggør access til størrelse
    int getSize()
    {
        return size;
    }

    int getSpin(Point2D p)
    {
        return spins[p.x][p.y];
    }

    double getB()
    {
        return B;
    }

    void setB(double argB)
    {
        B = argB;
    }

    void stepForward()
    {
        step++;
    }

    // initialisér med tilfældige spins
    void randomize()
    {
        for (std::vector<int> &row : spins)
        {
            for (int &spin : row)
            {
                spin = (distReal(rng) < 0.5) ? -1 : 1;
            }
        }
        step = 0;
    }

    long int getStep()
    {
        return step;
    }

    // Flip spin ved et givent koordinat
    void flipSpin(Point2D p)
    {
        spins[p.x][p.y] *= -1;
    }

    // Vælger et tilfældigt koordinat
    Point2D getRandomCoord()
    {
        return Point2D{distCoord(rng), distCoord(rng)};
    }

    void print()
    {
        for (const std::vector<int> &row : spins)
        {
            for (const int &spin : row)
            {
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
                std::cout << (spins[n][m] == 1 ? "██" : "░░");
            }
            std::cout << "\n";
        }
    }

    double magnetization()
    {
        int totalSpin = 0;
        for (const std::vector<int> &row : spins)
        {
            for (const int &spin : row)
            {
                totalSpin += spin;
            }
        }

        return static_cast<double>(totalSpin) / (size * size);
    }

    std::vector<int> ClusterSizes()
    {
        // Get the size of the lattice
        int L = getSize();

        // Initialize visited vector to keep track of visited sites
        std::vector<bool> visited(L * L, false);
        std::vector<int> cluster_sizes;

        // Loop over entire lattice
        for (int x = 0; x < L; x++)
        {
            for (int y = 0; y < L; y++)
            {

                // index of the current site (1D)
                int idx = x * L + y;

                // Skip site if it has already been visited
                if (visited[idx])
                    continue;

                // Get the spin of the current site
                int spin = getSpin({x, y});

                int cluster_size = 0;

                // Depth-First Search (DFS) stack
                std::vector<Point2D> stack;
                stack.push_back({x, y});
                visited[idx] = true;

                // DFS loop
                while (!stack.empty())
                {
                    auto p = stack.back();
                    stack.pop_back();
                    cluster_size++;

                    // Check all neighbors of the current point
                    for (auto interaction : getInteractions(p))
                    {
                        Point2D n = interaction.neighbor;
                        // Calculate the index of the neighbor (1D)
                        int nidx = n.x * L + n.y;

                        // If the neighbor has the same spin and has not been visited, add it to the stack
                        if (!visited[nidx] && getSpin(n) == spin)
                        {
                            // Mark the neighbor as visited and add it to the stack
                            visited[nidx] = true;
                            stack.push_back(n);
                        }
                    }
                }
                // After the DFS is complete, we have the size of the cluster
                cluster_sizes.push_back(cluster_size);
            }
        }

        return cluster_sizes;
    }

    double meanClusterSize()
    {
        std::vector<int> sizes = ClusterSizes();
        double num = 0.0;
        double den = 0.0;

        for (int s : sizes)
        {
            num += s * s;
            den += s;
        }

        return num / den;
    }

    measurement2D measure()
    {
        return measurement2D{step, magnetization(), meanClusterSize()};
    }

    virtual std::vector<interaction2D> getInteractions(Point2D p) = 0;
    virtual double deltaH(Point2D p) = 0;
};

// square structure with normal periodic boundary conditions
class SquareLattice2D : public Lattice2D
{
public:
    SquareLattice2D(int sizeArg, double BArg = 0) : Lattice2D(sizeArg, BArg) {}

    std::vector<interaction2D> getInteractions(Point2D p) override
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

        for (const interaction2D &interaction : getInteractions(p))
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

    std::vector<interaction2D> getInteractions(Point2D p) override
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
        double H = -B * getSpin(p);

        for (const interaction2D &interaction : getInteractions(p))
        {
            H -= getSpin(p) * getSpin(interaction.neighbor) * interaction.J; // Her mangler p's eget spin, som skal indgå i beregningen af dH
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

    std::vector<interaction2D> getInteractions(Point2D p) override
    {
        std::vector<interaction2D> interactions;
        double J = -1; // Interaction strength in units of k_B (J/k_B)

        if (p.x % 2 == 1)
        {
            interactions.push_back(interaction2D{Point2D{(p.x - 1) % size, (p.y) % size}, J});
            interactions.push_back(interaction2D{Point2D{(p.x + 1) % size, (p.y) % size}, J});
            interactions.push_back(interaction2D{Point2D{(p.x - 1) % size, (p.y + 1) % size}, J});
            interactions.push_back(interaction2D{Point2D{(p.x - 1) % size, (p.y - 1) % size}, J});
            interactions.push_back(interaction2D{Point2D{(p.x) % size, (p.y + 1) % size}, J});
            interactions.push_back(interaction2D{Point2D{(p.x) % size, (p.y - 1) % size}, J});
        }
        else
        {
            interactions.push_back(interaction2D{Point2D{(p.x - 1) % size, (p.y) % size}, J});
            interactions.push_back(interaction2D{Point2D{(p.x + 1) % size, (p.y) % size}, J});
            interactions.push_back(interaction2D{Point2D{(p.x) % size, (p.y + 1) % size}, J});
            interactions.push_back(interaction2D{Point2D{(p.x) % size, (p.y - 1) % size}, J});
            interactions.push_back(interaction2D{Point2D{(p.x + 1) % size, (p.y + 1) % size}, J});
            interactions.push_back(interaction2D{Point2D{(p.x + 1) % size, (p.y - 1) % size}, J});
        }
        return interactions;
    }

    double deltaH(Point2D p) override
    {
        double H = -B;

        for (const interaction2D &interaction : getInteractions(p))
        {
            H -= getSpin(interaction.neighbor) * interaction.J; // Her er p's eget spin faktoriseret ud, se i beregningen af dH
        }

        double dH = -2 * getSpin(p) * H;

        return dH;
    }
};

#endif // LATTICES2D_HPP