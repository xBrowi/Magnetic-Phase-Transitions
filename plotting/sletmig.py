import numpy as np
import matplotlib.pyplot as plt
import matplotlib.cm as cm
import matplotlib.colors as mcolors

from scipy.optimize import curve_fit


data = np.genfromtxt("output/fourier_sweep_output.csv",delimiter=",",skip_header=1)
plotdata = data[5:-1]
N = np.sqrt(len(plotdata))
plotdata2D = np.split(plotdata,N)
plotdata1D = plotdata2D[int(N/2)]
plotdata1Dwrapped = np.fft.fftshift(plotdata1D)


X = np.arange(len(plotdata1Dwrapped))

plt.plot(X,plotdata1Dwrapped)
plt.savefig("sletmig.png")