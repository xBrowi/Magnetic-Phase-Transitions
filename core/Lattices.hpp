#ifndef LATTICES_HPP
#define LATTICES_HPP

#include <vector>
#include <iostream>
#include <random>
#include <fftw3.h>



// A measurement class for storing the results of MC simulations at a timestep.
struct Measurement
{
    long int step;
    double magnetization;
    double meanClusterSize;
};

// identifier for various 2D lattice types
enum class LatticeType
{
    Square,
    FunkySquare,
    Triangle,
    Cubic,
};

struct Interaction
{
    int neighbor;
    double J; // OBS: IN UNITS OF K_B (J/K_B)
};

struct TrackFourier
{
    int counter;
    stid::vector<double> normSum; 
};




class Lattice
{
protected:    
    int size;
    long int step;
    std::vector<int> spins;
    double B;
    TrackFourier fourier;

    // Random number generator (rng)
    std::mt19937 rng{std::random_device{}()};
    std::uniform_int_distribution<int> distCoord;
    std::uniform_int_distribution<int> distIndex;
    std::uniform_real_distribution<double> distReal{0.0, 1.0};

public:

    int getSize()
    {
        return size;
    }

    std::vector<int> getConfiguration()
    {
        return spins;
    }

    int getSpinsSize()
    {
        return spins.size();
    }

    int getSpin(int index)
    {
        return spins[index];
    }

    void flipSpin(int index)
    {
        spins[index] *= -1;
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

    long int getStep()
    {
        return step;
    }

    void randomize()
    {
        for (int &s : spins)
        {
            s = (distReal(rng) < 0.5) ? 1 : -1;
        }
        step = 0;
    }

    int getRandomLatticeIndex()
    {
        return distIndex(rng);
    }

    double magnetization() //outdated, use fourier 
    {
        int totalSpin = 0;
        for (int s : spins)
        {
            totalSpin += s;
        }

        return static_cast<double>(totalSpin) / (size * size);
    }

    virtual double deltaH(int p) = 0;
    virtual std::vector<Interaction> getInteractions(int p) = 0;

    // generalized cluster size calculation for any lattice type and any dimensionality

    std::vector<int> ClusterSizes()
    {
        int L = spins.size();
        std::vector<bool> visited(L, false);
        std::vector<int> cluster_sizes;

        for (int i = 0; i < L; i++)
        {
            if (visited[i])
                continue;

            int spin = spins[i];
            int cluster_size = 0;

            std::vector<int> stack;
            stack.push_back(i);
            visited[i] = true;

            while (!stack.empty())
            {
                int currentIndex = stack.back();
                stack.pop_back();
                cluster_size++;

                for (const Interaction &interaction : getInteractions(currentIndex))
                {
                    int neighborIndex = interaction.neighbor;
                    if (!visited[neighborIndex] && spins[neighborIndex] == spin)
                    {
                        visited[neighborIndex] = true;
                        stack.push_back(neighborIndex);
                    }
                }
            }

            cluster_sizes.push_back(cluster_size);
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

    Measurement measure()
    {
        return measureOld(); // giver sig selv
        //return measureFourier();
    }


    void measureOld()
    {
        return Measurement{step, magnetization(), meanClusterSize()};
    }

    void measureFourier()
    {
        //kopieret fra test:
        //alloker skidtet til Fourierificering
    int N = size;
    fftw_complex *in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N * N);
    fftw_complex *out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N * N);

    //OBS!!!!! planen burde skabes under initialisering af latticen, og bare genbruges. ryk den her ud i constructoren? (*)
    fftw_plan p = fftw_plan_dft_2d(N, N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);     

    //fyld input arrayet med spin-konfigurationen
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            in[i*N + j][0] = spins[i * N + j]; // real part
            in[i*N + j][1] = 0.0; // imag part
        }
    }
    //kør magien
    fftw_execute(p);
    //output Fourier transformen til trackeren (skrevet med god gammel python syntax)
    out *= 1/(fourier.counter+1);
    fourier.koefficienter *= fourier.counter/(fourier.counter+1);
    fourier.koefficienter += out;
    fourier.counter++;


    //frigør hukommelsen
    fftw_destroy_plan(p); //(*) og den her ud ved destruktion af latticen. ca. 2x speedup?
    fftw_free(in);
    fftw_free(out);
    }
};
    

#endif // LATTICES_HPP