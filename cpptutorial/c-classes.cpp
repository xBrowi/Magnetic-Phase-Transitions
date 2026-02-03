// Classes er ++'et i C++. Det er en måde at definere egne datatyper med tilhørende funktioner (man kalder dem metoder når de er inde i en class eller struct). (måske en bestemt gitterstruktur!!)

#include <iostream>
#include <vector>

void printVector(const std::vector<int>& vec) {
    for (int val : vec) {
        std::cout << val << " ";
    }
    std::cout << "\n";
}

struct Point2D { // struct er næsten det samme som class, men beregnet til simple datatyper. Medlemmer er public som standard. (alt kan tilgås udefra)
    int x, y; // koordinater

    // metode til at udskrive punktets koordinater
    void print() {
        std::cout << "Point2D(" << x << ", " << y << ")\n";
    }
};

// kubisk 2d gitter class
class Grid2D {
private: // private medlemmer (ikke tilgængelige udefra)
    int size;
    std::vector<std::vector<int>> data; // 2d vector til at gemme gitterdata

public: // offentlige medlemmer (tilgængelige udefra)

    // constructor (kaldes når et objekt af klassen oprettes)
    Grid2D(int sizeIn) {
        size = sizeIn;
        data.resize(size, std::vector<int>(size, 0)); // initialiser 2d vector med nuller
    }

    // metode til at sætte en værdi i gitteret
    void setValue(Point2D p, int value) {
        if (p.x >= 0 && p.x < size && p.y >= 0 && p.y < size) {
            data[p.x][p.y] = value;
        }
    }

    // metode til at få en værdi fra gitteret
    int getValue(Point2D p) {
        if (p.x >= 0 && p.x < size && p.y >= 0 && p.y < size) {
            return data[p.x][p.y];
        }
        return 0; // returner 0 hvis uden for grænserne
    }

    // metode til at få størrelsen af gitteret
    int getSize() {
        return size;
    }

    // metode til at udskrive hele gitteret (kun for små størrelser)
    void print() {
        for (int x = 0; x < size; x++) {
            for (int y = 0; y < size; y++) {
                std::cout << data[x][y] << " ";
            }
            std::cout << "\n";
        }
    }

    // metode til at få koordinater for naboer til et punkt (bliver nok ret nyttigt!!)
    std::vector<Point2D> getNeighbors(Point2D p) {
        std::vector<Point2D> neighbors;
        if (p.x > 0) neighbors.push_back(Point2D{p.x - 1, p.y});
        if (p.x < size - 1) neighbors.push_back(Point2D{p.x + 1, p.y});
        if (p.y > 0) neighbors.push_back(Point2D{p.x, p.y - 1});
        if (p.y < size - 1) neighbors.push_back(Point2D{p.x, p.y + 1});
        return neighbors;
    }

}; // semicolon fordi idk

int main() {

    Point2D myPoint;
    myPoint.x = 4;
    myPoint.y = 5;

    myPoint.print();

    // opret et 5x5 gitter
    Grid2D grid(5);

    // sæt nogle værdier i gitteret
    grid.setValue(Point2D{1, 1}, 1);
    grid.setValue(Point2D{2, 2}, 2);
    grid.setValue(Point2D{3, 3}, 3);

    // udskriv gitteret
    std::cout << "Grid contents:\n";
    grid.print();

    // få og udskriv naboerne til punktet (2,2)
    Point2D p{2, 2};
    std::vector<Point2D> neighbors = grid.getNeighbors(p);
    std::cout << "\nNeighbors of Point2D(2, 2):\n";
    for (Point2D neighbor : neighbors) {
        neighbor.print();
    }

    // sæt alle naboer til (3,1) til værdien 5
    Point2D q{3, 1};
    std::vector<Point2D> neighborsQ = grid.getNeighbors(q);
    for (Point2D neighbor : neighborsQ) {
        grid.setValue(neighbor, 5);
    }

    std::cout << "\nGrid contents after setting neighbors of (3,1) to 5:\n";
    grid.print();

    return 0;
}