compile:
g++ (filNavn).cpp -o filNavn -O3 -pthread -lfftw3

run:
./filNavn

OBS: pga fftwPlannerMutex i 1D og 2D tror jeg at ens computer pisser i bukserne hvis vi kører 1D og 2D samtidigt. ikke gør det lol.

fourier: brug library fftw (3)