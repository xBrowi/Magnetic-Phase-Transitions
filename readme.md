compile:
g++ (filNavn).cpp -o filNavn -O3

run:
./filNavn

fourier:
library hedder fftw3
på linux ligger det i /include allerede, på windows måske tilføj flaget:
g++ (filNavn).cpp -o filNavn -O3 -pthread -lfftw3
kommenter ud i core/MonteCarlo.hpp hvis i vil køre noget andet, lige nu er den overflødig da den ikke bliver brugt i measurement endnu