#include <random>
#include <iostream>
#include <vector>
#include <time.h>

int main() {

    clock_t timestart = clock();
    // Opret random number generator
    std::random_device rd; // få tilfældig seed fra hardware (hvis muligt)
    std::mt19937 gen(rd()); // Mersenne Twister generator initialiseret med seed

    // Definer fordeling (her uniform distribution mellem 1 og 100)
    std::uniform_int_distribution<> distr(1, 10000);

    long long sum = 0;
    // generer 100000000 tilfældige heltal
    for (int n = 0; n < 100000000; ++n) {
        sum += distr(gen);
    }
    clock_t timeend = clock();
    
    // Udskriv summen
    std::cout << "Sum of random integers: " << sum << std::endl;

    double timeused = double(timeend - timestart) / CLOCKS_PER_SEC;
    std::cout << "Time used: " << timeused << " seconds\n";

    return 0;
}