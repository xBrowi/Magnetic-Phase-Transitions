#ifndef LATTICES1D_HPP
#define LATTICES1D_HPP

#include "Lattices.hpp"
#include <mutex>

inline std::mutex &fftwPlannerMutex1D()
{
    static std::mutex mtx;
    return mtx;
}

struct Point1D
{
    int x;
};

// an interaction class, storing the neighbors coordinates and the coupling strength
struct interaction1D
{
    Point1D neighbor;
    double J; // OBS: IN UNITS OF K_B (J/K_B)
};

class Lattice1D : public Lattice
{

protected:
    // Jeg tror, man normalt kalder spins for spinConfig. Værd at overveje navneskift
    
        fftw_complex *in = nullptr;
        fftw_complex *out = nullptr;
        fftw_plan p = nullptr;

public:
    // Constructor: initialisér et gitter med alle spins opad (+1)
    Lattice1D(int sizeArg, double argB = 0)
    {
        size = sizeArg;
        B = argB;
        step = 0;
        spins = std::vector<int>(sizeArg, 1);
        distIndex = std::uniform_int_distribution<int>(0, static_cast<int>(spins.size() - 1));
        measurementTracker = {0,std::vector<double>(sizeArg,0),std::vector<double>(sizeArg,0),0,0,0,0};
    }


    void initializeFourier()
    {
        int N = size;
        in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
        out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
        {
            std::lock_guard<std::mutex> lock(fftwPlannerMutex1D());
            p = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
        }
    }

    void freeFourier()
    {
        {
            std::lock_guard<std::mutex> lock(fftwPlannerMutex1D());
            if (p != nullptr)
            {
                fftw_destroy_plan(p);
                p = nullptr;
            }
        }

        if (in != nullptr)
        {
            fftw_free(in);
            in = nullptr;
        }

        if (out != nullptr)
        {
            fftw_free(out);
            out = nullptr;
        }
    }

    int getSpin(Point1D p)
    {
        return spins[p.x];
    }

    void flipSpin(Point1D p)
    {
        spins[p.x] *= -1;
    }

    Point1D getCoordFromIndex(int index)
    {
        return Point1D{index};
    }

    // Vælger et tilfældigt koordinat
    Point1D getRandomCoord()
    {
        return getCoordFromIndex(getRandomLatticeIndex());
    }

    void print()
    {
        for (int i = 0; i < size; i++)
        {
            int spin = getSpin(getCoordFromIndex(i));
            std::cout << (spin == 1 ? "██" : "░░");
        }
        std::cout << "\n";
    }

    virtual std::vector<interaction1D> getInteractions1D(Point1D p) = 0;
    virtual double deltaH(Point1D p) = 0;

    double deltaH(int index) override
    {
        Point1D p = getCoordFromIndex(index);
        return deltaH(p);
    }

    std::vector<Interaction> getInteractions(int index) override
    {
        Point1D p = getCoordFromIndex(index);
        std::vector<interaction1D> interactions1D = getInteractions1D(p);
        std::vector<Interaction> interactions;

        for (const interaction1D &interaction : interactions1D)
        {
            int neighborIndex = interaction.neighbor.x;
            interactions.push_back(Interaction{neighborIndex, interaction.J}); //ret langsomt, what can you do
        }

        return interactions;
    }

    std::vector<double> FourierNormKvadrat(fftw_complex* out)
    {
        std::vector<double> norms(size);
        for (int i = 0; i < size; i++)
        {
            norms[i] = out[i][0] * out[i][0] + out[i][1] * out[i][1];
        }
        return norms;
    }

    void updateMeasurementTracker()
    {
        int N = size;

        measurementTracker.counter++;
        
        double hamiltoncontributions = 0;
        //fyld input arrayet med spin-konfigurationen
        for (int i = 0; i < N; i++) {
        
            in[i][0] = spins[i]; // real part
            in[i][1] = 0.0; // imag part
            for ( interaction1D &interaction : getInteractions1D({i}))
            {
                double hamiltonContribution = -getSpin({i}) * getSpin(interaction.neighbor) * interaction.J / 2; // Hamiltonian contribution from this interaction (divided by 2 to avoid double counting)
                hamiltoncontributions += hamiltonContribution; // Hamiltonian sum
            }
        
        }
        measurementTracker.hamiltonSum += hamiltoncontributions;
        measurementTracker.hamiltonKvadratSum += hamiltoncontributions * hamiltoncontributions;
        measurementTracker.hamiltonKubeSum += hamiltoncontributions * hamiltoncontributions * hamiltoncontributions;
        measurementTracker.hamiltonKvadratKvadratSum += hamiltoncontributions * hamiltoncontributions * hamiltoncontributions * hamiltoncontributions;
        
        //kør magien
        fftw_execute(p);
        //output Fourier transformen til trackeren (skrevet med god gammel python syntax)

        std::vector<double> normKvadrat = FourierNormKvadrat(out);

        for (int i = 0; i < N; i++) {
            measurementTracker.normKvadratSum[i] += normKvadrat[i];
            measurementTracker.normKvadratKvadratSum[i] += normKvadrat[i] * normKvadrat[i];
        }

        measurementTracker.magnetiseringSum += std::sqrt(normKvadrat[0]) / double(N); // magnetisering er norm af k=0 komponenten, normaliseret
        measurementTracker.magnetiseringKvadratSum += (normKvadrat[0]) / (double(N) * double(N)); // kvadratet af magnetiseringen
        measurementTracker.magnetiseringKubeSum += (normKvadrat[0] * std::sqrt(normKvadrat[0])) / (double(N) * double(N) * double(N)); // kuben af magnetiseringen, normaliseret
        measurementTracker.magnetiseringKvadratKvadratSum += (normKvadrat[0] * normKvadrat[0]) / (double(N) * double(N) * double(N) * double(N)); // kvadratet af magnetiseringen, kvadreret igen
        
        //std::cout << "magnetisering, varians og variansvarians, " << normKvadrat[0] << ", " << measurementTracker.magnetiseringKvadratSum << ", " << measurementTracker.magnetiseringKvadratKvadratSum << "\n";






    }

};

//free boundary
class FreeLattice1D : public Lattice1D
{
public:
    FreeLattice1D(int sizeArg, double BArg = 0) : Lattice1D(sizeArg, BArg) {}

    std::vector<interaction1D> getInteractions1D(Point1D p) override
    {
        std::vector<interaction1D> interactions;
        double J = 1; // Interaction strength in units of k_B (J/k_B)

        if (p.x > 0)
            interactions.push_back(interaction1D{Point1D{p.x - 1}, J});
        if (p.x < size - 1)
            interactions.push_back(interaction1D{Point1D{p.x + 1}, J});

        return interactions;
    }

    double deltaH(Point1D p) override
    {
        double H = -B; // B is magnetic moment times magnetic field

        for (const interaction1D &interaction : getInteractions1D(p))
        {
            H -= getSpin(interaction.neighbor) * interaction.J; // Her mangler p's eget spin, som skal indgå i beregningen af dH
        }

        double dH = -2 * getSpin(p) * H;

        return dH;
    }
};

#endif // LATTICES1D_HPP