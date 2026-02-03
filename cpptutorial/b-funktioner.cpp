#include <iostream>
#include <vector>

// funktioner defineres med return type, navn og parametre i parentes
int add(int a, int b) {
    return a + b; // returnerer summen af a og b
}

// void betyder at funktionen ikke returnerer noget 
void printVector(const std::vector<int>& vec) {
    for (int val : vec) {
        std::cout << val << " ";
    }
    std::cout << "\n";
}

// REFERENCES. Genial ting i C++, som ikke findes i Python. Bruges til at undgå kopiering af store datatyper, eller for at ændre den originale variabel.
// references betyder at man ikke give en kopi, men den faktiske variabel. Her er to funktioner, den ene bruger en reference, og den anden ikke. 
void makeZero(int x) {
    x = 0;
    std::cout << "Inside makeZero (by value): " << x << "\n"; // ændrer kun den lokale kopi
}

// her bruges & for at angive at x er en reference til den originale variabel
void makeZeroRef(int& x) {
    x = 0;
    std::cout << "Inside makeZeroRef (by reference): " << x << "\n"; // ændrer den originale variabel
}


// "const" betyder at parameteren ikke kan ændres inde i funktionen. gør ikke noget for resultatet, men kan gøre koden mere sikker og lettere at forstå.
// dette er eksempelvis ikke tilladt:
/*
void wrongFunction(const int& x) {
    x = x + 1; // fejl: kan ikke ændre en const reference
}
*/






int main() {
    int sum = add(5, 10); // kalder funktionen add med 5 og 10 som argumenter
    std::cout << "Sum: " << sum << "\n";

    std::vector<int> myVector = {1, 2, 3, 4, 5};
    std::cout << "Vector elements: ";
    printVector(myVector); // kalder funktionen printVector for at udskrive elementerne i myVector

    // References:
    int number = 42;
    std::cout << "før makeZero: " << number << "\n";
    makeZero(number); // kalder makeZero, som ikke ændrer den originale variabel
    std::cout << "efter makeZero: " << number << "\n";
    makeZeroRef(number); // kalder makeZeroRef, som ændrer den originale variabel
    std::cout << "efter makeZeroRef: " << number << "\n";

    return 0;
}