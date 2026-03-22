import numpy as np
import matplotlib.pyplot as plt
import matplotlib.cm as cm
import matplotlib.colors as mcolors

from scipy.optimize import curve_fit


parameters = np.genfromtxt("output/FourierFrustreret/parameters.csv", delimiter=",", skip_header=0)
measurements = np.genfromtxt("output/FourierFrustreret/measurements.csv", delimiter=",", skip_header=0)
kvadratFFTs = np.genfromtxt("output/FourierFrustreret/kvadratFFT.csv", delimiter=",", skip_header=0)[:,:-1] #-1 fordi vi har et trailing NaN fra trailing komma
kvadratFFTVars = np.genfromtxt("output/FourierFrustreret/kvadratFFTvariance.csv", delimiter=",", skip_header=0)[:,:-1]

simCount = len(parameters)

sizes = parameters[:,0]
temps = parameters[:,1]

counts = measurements[:,0]
magnetiseringer = measurements[:,1]
susceptibiliteter = measurements[:,2]
susceptibilitetvarianser = measurements[:,3]
energier = measurements[:,4]
varmekapaciteter = measurements[:,5]
varmevarianser = measurements[:,6]

kvadratFFT1Ds = []
kvadratFFTVar1Ds = []


#1D FFT
for i in range(simCount):
    
    kvadratFFT2D = np.fft.fftshift(np.split(kvadratFFTs[i], sizes[i]))
    kvadratFFT1D = kvadratFFT2D[int(sizes[i]//2)]  # Tag den midterste række (k_y = 0)
    kvadratFFT1Ds.append(kvadratFFT1D)
    

    kvadratFFTVar2D = np.fft.fftshift(np.split(kvadratFFTVars[i], sizes[i]))
    kvadratFFTVar1D = kvadratFFTVar2D[int(sizes[i]//2)]  # Tag den midterste række (k_y = 0)
    kvadratFFTVar1Ds.append(kvadratFFTVar1D)

kvadratFFT1Dusikkerheder = np.sqrt(kvadratFFTVar1Ds)
kvadratFFT1Dusikkerheder /= np.sqrt(counts)[:, np.newaxis]  # Usikkerhed på gennemsnittet er standardafvigelsen delt med kvadratroden af antallet af målinger

gammas = []
gammaUsikkerheder = []

#cauchy fordeling med offset for at plotte fittet
def cauchy_offset(x, A, gamma, k):
    return A/(np.pi*gamma*(1 + ((x)/gamma)**2)) + k

# remove middle point for at undgå singularitet i cauchy fit
middle_indices = [len(kvadratFFT1D)//2 for kvadratFFT1D in kvadratFFT1Ds]
kvadratFFT1Ds = [np.delete(kvadratFFT1Ds[i], middle_indices[i]) for i in range(simCount)]
kvadratFFT1Dusikkerheder = [np.delete(kvadratFFT1Dusikkerheder[i], middle_indices[i]) for i in range(simCount)]

# alle ffterne (subplots over hinanden med samme x akse?)
for i in range(simCount):  # Adjust the range as needed
    X = np.arange(-sizes[i]//2, sizes[i]//2)
    X = np.delete(X, len(X)//2)  # Fjern det midterste punkt for at undgå singularitet i cauchy fit
    p0 = [max(kvadratFFT1Ds[i]), 10, min(kvadratFFT1Ds[i])]  # Initiale gæt for A, gamma og k
    
    #par, cov = curve_fit(cauchy_offset, X, kvadratFFT1Ds[i], p0=p0, maxfev=50000)
    #gammaUsikkerheder.append(np.sqrt(cov[1][1]))
    #gammas.append(par[1])


    X_plot = np.linspace(-sizes[i]//2, sizes[i]//2, 1000)

    plt.errorbar(X, kvadratFFT1Ds[i], yerr=kvadratFFT1Dusikkerheder[i], fmt='.', label=f"Data Temp {temps[i]}")
    #plt.plot(X_plot, cauchy_offset(X_plot, *par), label=f"Cauchy Fit Temp {temps[i]}")

    #plot startgæt
    #plt.plot(X, cauchy_offset(X, *p0), label=f"Startgæt Temp {temps[i]}")
    
#plt.xlim(-sizes[i]//6, sizes[i]//6)
plt.ylim(0, 1e5)

plt.xlabel(r"$k_x$")
plt.ylabel(r"$|\tilde{m}(k_x, k_y=0)|^2$")
plt.title("1D Fourier som funktion af tid")
plt.legend()
plt.savefig("output/Fourier/1D_Fouriers.png",dpi =600)
plt.close()

# magnetisering som funktion af temp
plt.errorbar(temps, magnetiseringer, yerr=np.sqrt(susceptibiliteter), fmt='o-')
plt.xlabel("Temperatur")
plt.ylabel("Magnetisering")
plt.title("Magnetisering som funktion af temperatur")
plt.savefig("output/Fourier/Magnetisering.png")
plt.close()
# susceptibilitet som funktion af temp 
plt.errorbar(temps, susceptibiliteter, yerr=np.sqrt(susceptibilitetvarianser), fmt='o-')
plt.xlabel("Temperatur")
plt.ylabel("Susceptibilitet")
plt.title("Susceptibilitet som funktion af temperatur")
plt.savefig("output/Fourier/susceptibilitet.png")
plt.close()
# energi som funktion af temp
plt.errorbar(temps, energier, yerr=np.sqrt(varmekapaciteter), fmt='o-')
plt.xlabel("Temperatur")
plt.ylabel("Energi")
plt.title("Energi som funktion af temperatur")
plt.savefig("output/Fourier/Energi.png")
plt.close()
# varmekapacitet som funktion af temp
plt.errorbar(temps, varmekapaciteter, yerr=np.sqrt(varmevarianser), fmt='o-')
plt.xlabel("Temperatur")
plt.ylabel("Varmekapacitet")
plt.title("Varmekapacitet som funktion af temperatur")
plt.savefig("output/Fourier/Varmekapacitet.png")
plt.close()
# korrelationslængde som funktion af temp

plt.errorbar(temps, 1/np.array(gammas), yerr=1/np.array(gammaUsikkerheder), fmt='o-') #usikkerheder skal måske bare være usikkerheden på gamma fra python fit?
plt.xlabel("Temperatur")
plt.ylabel("Korrelationslængde (gamma)")
plt.title("Korrelationslængde som funktion af temperatur")
plt.savefig("output/Fourier/Korrelationslaengde.png")
plt.close()

