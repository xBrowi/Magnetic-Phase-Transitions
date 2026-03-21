import numpy as np
import matplotlib.pyplot as plt
import matplotlib.cm as cm
import matplotlib.colors as mcolors

from scipy.optimize import curve_fit


parameters = np.genfromtxt("output/Fourier1D/parameters.csv", delimiter=",", skip_header=0)
measurements = np.genfromtxt("output/Fourier1D/measurements.csv", delimiter=",", skip_header=0)
kvadratFFTs = np.genfromtxt("output/Fourier1D/kvadratFFT.csv", delimiter=",", skip_header=0)[:,:-1] #-1 fordi vi har et trailing NaN fra trailing komma
kvadratFFTVars = np.genfromtxt("output/Fourier1D/kvadratFFTvariance.csv", delimiter=",", skip_header=0)[:,:-1]

simCount = len(parameters)

sizes = parameters[:,0]
temps = parameters[:,1]

counts = measurements[:,0]
magnetiseringer = measurements[:,1]
susceptibiliteter = measurements[:,2]
energier = measurements[:,3]
varmekapaciteter = measurements[:,4]

#1D FFT
for i in range(simCount):
    
    kvadratFFT1D = np.fft.fftshift(kvadratFFTs[i])  # Tag den midterste række (k_y = 0)
    kvadratFFTs[i] = kvadratFFT1D
    

    kvadratFFTVar1D = np.fft.fftshift(kvadratFFTVars[i])  # Tag den midterste række (k_y = 0)
    kvadratFFTVars[i] = kvadratFFTVar1D

kvadratFFTusikkerheder = np.sqrt(kvadratFFTVars)
kvadratFFTusikkerheder /= np.sqrt(counts)[:, np.newaxis]  # Usikkerhed på gennemsnittet er standardafvigelsen delt med kvadratroden af antallet af målinger

gammas = []

#cauchy fordeling med offset for at plotte fittet
def cauchy_offset(x, A, gamma, k):
    return A/(np.pi*gamma*(1 + ((x)/gamma)**2)) + k

# remove middle point for at undgå singularitet i cauchy fit
middle_indices = [len(kvadratFFT)//2 for kvadratFFT in kvadratFFTs]
kvadratFFTs = [np.delete(kvadratFFTs[i], middle_indices[i]) for i in range(simCount)]
kvadratFFTusikkerheder = [np.delete(kvadratFFTusikkerheder[i], middle_indices[i]) for i in range(simCount)]

# alle ffterne (subplots over hinanden med samme x akse?)
for i in range(simCount):  # Adjust the range as needed
    X = np.arange(-sizes[i]//2, sizes[i]//2)
    X = np.delete(X, len(X)//2)  # Fjern det midterste punkt for at undgå singularitet i cauchy fit
    p0 = [max(kvadratFFTs[i])*1000, 5, 0]  # Initiale gæt for A, gamma og k
    par, cov = curve_fit(cauchy_offset, X, kvadratFFTs[i], p0=p0)
    gammas.append(par[1])


    X_plot = np.linspace(-sizes[i]//2, sizes[i]//2, 1000)

    plt.errorbar(X, kvadratFFTs[i], yerr=kvadratFFTusikkerheder[i], fmt='.', label=f"Data Temp {temps[i]}")
    plt.plot(X_plot, cauchy_offset(X_plot, *par), label=f"Cauchy Fit Temp {temps[i]}")

    #plot startgæt
    #plt.plot(X, cauchy_offset(X, *p0), label=f"Startgæt Temp {temps[i]}")
    
plt.xlim(-sizes[i]//6, sizes[i]//6)
#plt.ylim(0, 1e5)

plt.xlabel(r"$k_x$")
plt.ylabel(r"$|\tilde{m}(k_x, k_y=0)|^2$")
plt.title("1D Fourier som funktion af tid")
plt.legend()
plt.savefig("output/Fourier1D/1D_Fouriers.png",dpi =600)
plt.close()

# magnetisering som funktion af temp
plt.plot(temps, magnetiseringer, 'o-')
plt.xlabel("Temperatur")
plt.ylabel("Magnetisering")
plt.title("Magnetisering som funktion af temperatur")
plt.savefig("output/Fourier1D/Magnetisering.png")
plt.close()
# susceptibilitet som funktion af temp 
plt.plot(temps, susceptibiliteter, 'o-')
plt.xlabel("Temperatur")
plt.ylabel("Susceptibilitet")
plt.title("Susceptibilitet som funktion af temperatur")
plt.savefig("output/Fourier1D/susceptibilitet.png")
plt.close()
# energi som funktion af temp
plt.plot(temps, energier, 'o-')
plt.xlabel("Temperatur")
plt.ylabel("Energi")
plt.title("Energi som funktion af temperatur")
plt.savefig("output/Fourier1D/Energi.png")
plt.close()
# varmekapacitet som funktion af temp
plt.plot(temps, varmekapaciteter, 'o-')
plt.xlabel("Temperatur")
plt.ylabel("Varmekapacitet")
plt.title("Varmekapacitet som funktion af temperatur")
plt.savefig("output/Fourier1D/Varmekapacitet.png")
plt.close()
# korrelationslængde som funktion af temp

plt.plot(temps, 1/np.array(gammas), 'o-')
plt.xlabel("Temperatur")
plt.ylabel("Korrelationslængde (gamma)")
plt.title("Korrelationslængde som funktion af temperatur")
plt.savefig("output/Fourier1D/Korrelationslaengde.png")
plt.close()

