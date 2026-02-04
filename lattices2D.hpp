#ifndef LATTICES2D_HPP
#define LATTICES2D_HPP

#include <vector>
#include <iostream>
#include <random>

// This is a 2D point class. The function print() prints its coordinates.
struct Point2D {
    int x, y;

    void print() {
        std::cout << "Point2D(" << x << ", " << y << ")\n";
    }
};

struct interaction2D {
    Point2D neighbor;
    double J; // OBS: IN UNITS OF K_B (J/K_B)
};

// General class for lattices. All other lattice classes should inherit these properties.
class Lattice2D {
// private is for stuff that should not be accessed, changed or de
protected:
    int size;
    std::vector<std::vector<int>> spins;

    // Random number generator (rng)
    std::mt19937 rng{std::random_device{}()};
    std::uniform_int_distribution<int> distCoord;
    std::uniform_real_distribution<double> distReal{0.0, 1.0};

public:

    // Constructor: initialisér et gitter med alle spins opad (+1)
    Lattice2D(int sizeArg) {
        size = sizeArg;
        spins = std::vector<std::vector<int>> (sizeArg, std::vector<int>(sizeArg, 1)); 
        distCoord = std::uniform_int_distribution<int>(0, size - 1);
    }
    
    // Muliggør access til størrelse
    int getSize() {
        return size;
    }

    int getSpin(Point2D p) {
        return spins[p.x][p.y];
    }

    // initialisér med tilfældige spins
    void randomize() {
        for (std::vector<int>& row : spins) {
            for (int& spin : row) {
                spin = (distReal(rng) < 0.5) ? -1 : 1;
            }
        }
    }

    // Flip spin ved et givent koordinat
    void flipSpin(Point2D p) {
        spins[p.x][p.y] *= -1;
    }
    
    // Vælger et tilfældigt koordinat
    Point2D getRandomCoord() {
        return Point2D{distCoord(rng), distCoord(rng)};
    }

    void print() {
        for (const std::vector<int>& row : spins) {
            for (const int& spin : row) {
                std::cout << (spin == 1 ? "██" : "░░");
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }


    virtual std::vector<interaction2D> getInteractions(Point2D p) = 0;
    virtual double deltaH(Point2D p) = 0;

};




class SquareLattice2D : public Lattice2D {
public:

    SquareLattice2D(int sizeArg) : Lattice2D(sizeArg) {}

    std::vector<interaction2D> getInteractions(Point2D p) override {
        std::vector<interaction2D> interactions;
        double J = 1; // Interaction strength in units of k_B (J/k_B)

        if (p.x > 0) interactions.push_back(interaction2D{Point2D{p.x - 1, p.y}, J});
        if (p.x < size - 1) interactions.push_back(interaction2D{Point2D{p.x + 1, p.y}, J});
        if (p.y > 0) interactions.push_back(interaction2D{Point2D{p.x, p.y - 1}, J});
        if (p.y < size - 1) interactions.push_back(interaction2D{Point2D{p.x, p.y + 1}, J});

        return interactions;
    }



    double deltaH(Point2D p) override {
        double H = 0;
        
        for (const interaction2D& interaction : getInteractions(p)) {
            H -= getSpin(interaction.neighbor)*interaction.J;
        }

        double dH = -2*getSpin(p)*H;

        return dH;
    }


};




#endif // LATTICES2D_HPP