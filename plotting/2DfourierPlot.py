import os
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.cm as cm
import matplotlib.colors as mcolors

from scipy.optimize import curve_fit

def numeric_suffix_key(filename):
    stem = os.path.splitext(filename)[0]
    suffix = stem.rsplit("_", 1)[-1]
    return int(suffix) if suffix.isdigit() else float("inf")

parameterFiles = np.array(sorted(os.listdir("output/Fourier/WolffFerroFirkant/parameters"), key=numeric_suffix_key))
measurementFiles = np.array(sorted(os.listdir("output/Fourier/WolffFerroFirkant/measurements"), key=numeric_suffix_key))
kvadratFFTFiles = np.array(sorted(os.listdir("output/Fourier/WolffFerroFirkant/kvadratFFT"), key=numeric_suffix_key))
kvadratFFTVarFiles = np.array(sorted(os.listdir("output/Fourier/WolffFerroFirkant/kvadratFFTvariance"), key=numeric_suffix_key))

print("Parameterfiler:", parameterFiles)
print("Måledatafiler:", measurementFiles)
print("Kvadrat FFT filer:", kvadratFFTFiles)
print("Kvadrat FFT varians filer:", kvadratFFTVarFiles)

antalStørrelser = len(parameterFiles)



magnetiseringPlots = []
susceptibilitetPlots = []
energiPlots = []
varmekapacitetPlots = []
APlots = []
korrelationslængdePlots = []



for j in range(0,antalStørrelser):
    
    parameters = np.genfromtxt("output/Fourier/WolffFerroFirkant/parameters/" + parameterFiles[j], delimiter=",")[:,:-1]
    measurements = np.genfromtxt("output/Fourier/WolffFerroFirkant/measurements/" + measurementFiles[j], delimiter=",")[:,:-1]
    kvadratFFTs = np.genfromtxt("output/Fourier/WolffFerroFirkant/kvadratFFT/" + kvadratFFTFiles[j], delimiter=",")[:,:-1]
    kvadratFFTVars = np.genfromtxt("output/Fourier/WolffFerroFirkant/kvadratFFTvariance/" + kvadratFFTVarFiles[j], delimiter=",")[:,:-1]

    



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

        print(len(kvadratFFTs[i]))
        
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

    As = []

    # alle ffterne (subplots over hinanden med samme x akse?)
    for i in range(simCount):  # Adjust the range as needed
        X = np.arange(-sizes[i]//2, sizes[i]//2)
        X = np.delete(X, len(X)//2)  # Fjern det midterste punkt for at undgå singularitet i cauchy fit

        if (sizes[i]%2 == 1):
            X += 1 # Juster X for at sikre korrekt placering af punkterne efter fjernelse af midterste punkt

        X *= 2*np.pi/sizes[i]  # Skaler X for at få korrekte k-værdier
        

        p0 = [max(kvadratFFT1Ds[i]), 10, min(kvadratFFT1Ds[i])]  # Initiale gæt for A, gamma og k
        par, cov = curve_fit(cauchy_offset, X, kvadratFFT1Ds[i], p0=p0, maxfev=50000, bounds=([0, 0, -np.inf], [np.inf, np.inf, np.inf]))  # Tilføj bounds for at sikre positive A og gamma
        gammaUsikkerheder.append(np.sqrt(cov[1][1]))
        gammas.append(par[1])
        As.append(par[0])


        X_plot = np.linspace(-sizes[i]//2, sizes[i]//2, 1000)
        X_plot *= 2*np.pi/sizes[i]  # Skaler X_plot for at få korrekte k-værdier

        if(i == 4):
            plt.errorbar(X, kvadratFFT1Ds[i], yerr=kvadratFFT1Dusikkerheder[i], fmt='.', label=f"Data Temp {temps[i]}")
            plt.plot(X_plot, cauchy_offset(X_plot, *par), label=f"Cauchy Fit Temp {temps[i]}")

        #plot startgæt
        #plt.plot(X, cauchy_offset(X, *p0), label=f"Startgæt Temp {temps[i]}")
        
    #plt.xlim(-sizes[i]//6, sizes[i]//6)
    #plt.ylim(0, 1e5)

    plt.xlabel(r"$k_x$")
    plt.ylabel(r"$|\tilde{m}(k_x, k_y=0)|^2$")
    plt.title("1D Fourier som funktion af tid")
    plt.legend()
    #plt.ylim(0, np.max(kvadratFFT1Ds)*1.1)  # Juster y-aksen baseret på data
    plt.savefig(f"plots/Fourier/{j}_1D_Fouriers.png",dpi =600)
    plt.close()

    # magnetisering som funktion af temp
    plt.errorbar(temps, magnetiseringer, yerr=np.sqrt(susceptibiliteter/counts), fmt='-')
    plt.xlabel("Temperatur")
    plt.ylabel("Magnetisering")
    plt.title("Magnetisering som funktion af temperatur")
    magnetiseringPlots.append(plt.gcf())
    plt.savefig(f"plots/Fourier/{j}_Magnetisering.png")
    plt.close()
    # susceptibilitet som funktion af temp 
    plt.errorbar(temps, susceptibiliteter, yerr=np.sqrt(susceptibilitetvarianser/counts), fmt='-')
    plt.xlabel("Temperatur")
    plt.ylabel("Susceptibilitet")
    plt.title("Susceptibilitet som funktion af temperatur")
    plt.savefig(f"plots/Fourier/{j}_susceptibilitet.png", dpi=600)
    plt.close()
    # energi som funktion af temp
    plt.errorbar(temps, energier, yerr=np.sqrt(varmekapaciteter/counts), fmt='-')
    plt.xlabel("Temperatur")
    plt.ylabel("Energi")
    plt.title("Energi som funktion af temperatur")
    plt.savefig(f"plots/Fourier/{j}_Energi.png")
    plt.close()
    # varmekapacitet som funktion af temp
    plt.errorbar(temps, varmekapaciteter, yerr=np.sqrt(varmevarianser/counts), fmt='o-')
    plt.xlabel("Temperatur")
    plt.ylabel("Varmekapacitet")
    plt.title("Varmekapacitet som funktion af temperatur")
    plt.savefig(f"plots/Fourier/{j}_Varmekapacitet.png")
    plt.close()
    # A som funktion af T
    plt.errorbar(temps, As, fmt='o-') #usikkerheder skal måske bare være usikkerheden på A fra python fit?
    plt.xlabel("Temperatur")
    plt.ylabel("A")
    plt.title("A som funktion af temperatur")
    #plt.ylim(0, 1e8)
    plt.savefig(f"plots/Fourier/{j}_A.png")
    plt.close()

    # korrelationslængde som funktion af temp
    #print("As:", As)
    #print("Gammas:", gammas)
    #print("Gamma Usikkerheder:", gammaUsikkerheder)
    korrlængder = 1/np.array(gammas)
    korrUsikkerhed = np.array(gammaUsikkerheder) / np.array(gammas)**2  # Usikkerhed på 1/gamma

    plt.errorbar(temps, np.array(korrlængder), yerr=np.array(korrUsikkerhed), fmt='o-') #usikkerheder skal måske bare være usikkerheden på korrelationslængde fra python fit?
    plt.xlabel("Temperatur")
    plt.ylabel("Korrelationslængde (1/gamma)")
    plt.ylim(0, np.max(korrlængder)*1.1)  # Juster y-aksen baseret på data
    plt.title("Korrelationslængde som funktion af temperatur")
    plt.savefig(f"plots/Fourier/{j}_Korrelationslaengde.png")
    plt.close()