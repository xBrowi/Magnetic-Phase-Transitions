#include <iostream>
#include <vector>



// laver klasse til objekter (to integers)
struct point2D {
    int x,y;

    void print() {
        std::cout << "Point2D(" << x << ", " << y << ")\n";
    }
};







int main() {
    point2D p{2,4};

    p.print();
    
    return 0;
}





