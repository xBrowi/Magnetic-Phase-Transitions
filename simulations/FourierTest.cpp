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
    params.size = 20;
    params.temperature = 2.05;
    params.totalStepCount = 1e4;
    params.measurementInterval = 1000;
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

    //alloker skidtet til Fourierificering
    int N = lattice->getSize();
    fftw_complex *in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N * N);
    fftw_complex *out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N * N);
    fftw_plan p = fftw_plan_dft_2d(N, N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

    //transformér spin-konfigurationen til FFor now, we will use the Metropolis algorithm for all simulations. We can easily switch to Wolff or implement other algorithms in the future.ourier space
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            in[i*N + j][0] = lattice->getSpin({i,j}); // real part
            in[i*N + j][1] = 0.0; // imag part
        }
    }

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

    //print done
    //std::cout << "Done! Fourier transform output saved to output/fourier_output.csv\n";

    return 0;
}