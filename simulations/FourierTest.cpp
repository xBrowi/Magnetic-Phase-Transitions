#include "../core/MonteCarlo.hpp"
#include "../core/Lattices.hpp"

#include <vector>
#include <iostream>
#include <fstream>
#include <random>
#include <chrono>

//midlertidigt
#include <cmath>
#include <fftw3.h>

int main() 
{

    MCParameters params;
    params.latticeType = LatticeType::FunkySquare;
    params.size = 32;
    params.temperature = 3.29;
    params.totalStepCount = 1e5;
    params.measurementInterval = 1e5;
    params.randomize = true;
    params.printProgress = false;

    Lattice2D *lattice;
    lattice = new FunkySquareLattice2D(params.size);

    //step through the simulation to get a non-trivial spin configuration
    std::mt19937 rng{std::random_device{}()};
    std::uniform_real_distribution<double> distReal{0.0, 1.0};
    for (long int i = 0; i < params.totalStepCount; i++)
    {
        MCStep(*lattice, params.temperature, rng, distReal);
    }

    lattice->print();

    auto startTime = std::chrono::high_resolution_clock::now();
    
    //alloker skidtet til Fourierificering
    int N = lattice->getSize();
    fftw_complex *in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N * N);
    fftw_complex *out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N * N);
    fftw_plan p = fftw_plan_dft_2d(N, N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

    //fyld input arrayet med spin-konfigurationen
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            in[i*N + j][0] = lattice->getSpin({i,j}); // real part
            in[i*N + j][1] = 0.0; // imag part
        }
    }
    auto flag = std::chrono::high_resolution_clock::now();
    //kør magien
    fftw_execute(p);
    //output Fourier transformen
    std::ofstream outFile("output/fourier_output.csv");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            outFile << out[i*N + j][0] << " " << out[i*N + j][1] << "\n"; // real and imag parts
        }
    }
    outFile.close();    

    //frigør hukommelsen
    fftw_destroy_plan(p);
    fftw_free(in);
    fftw_free(out);

    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = endTime - startTime;
    std::cout << "Fourier transform completed in " << elapsed.count() << " seconds.\n";
    std::chrono::duration<double> elapsedFlag = endTime - flag;
    std::cout << "Fourier execution completed in " << elapsedFlag.count() << " seconds.\n";

    return 0;
}