#include <iostream>
#include <vector>

class Lattice {
public:
    virtual void displayInfo() = 0;
};

class SquareLattice : public Lattice {
public:
    void displayInfo() override {
        std::cout << "This is a Square Lattice.\n";
    }
};

class HexagonalLattice : public Lattice {
public:
    void displayInfo() override {
        std::cout << "This is a Hexagonal Lattice.\n";
    }
};

void printLatticeInfo(Lattice& lattice) {
    lattice.displayInfo();
}

int main() {
    SquareLattice square;
    HexagonalLattice hexagonal;

    printLatticeInfo(square);
    printLatticeInfo(hexagonal);

    return 0;
}