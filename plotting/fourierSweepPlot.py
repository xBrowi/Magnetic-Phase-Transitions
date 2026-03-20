import numpy as np
import matplotlib.pyplot as plt
import matplotlib.cm as cm
import matplotlib.colors as mcolors

from scipy.optimize import curve_fit


data = np.genfromtxt("output/fourier_sweep_output.csv", delimiter=",", skip_header=1)

'''
plotdata = data[31,5:-1] #-1 fordi vi har et trailing NaN fra trailing komma
#plotdata[0] = 0
print(plotdata)
print(plotdata.shape)

N = np.sqrt(len(plotdata))
plotdata2D = np.split(plotdata, len(plotdata)//N)
plotdatawrapped = np.fft.fftshift(plotdata2D)

cmap = cm.viridis

'''

plotdata = data[:,5:-1]

N = int(np.sqrt(len(plotdata[1])))
print(N)


plotdata1D = plotdata[:,(N*N//2):(N*N//2+N)]

for i in range(len(plotdata1D)):
    X = np.arange(len(plotdata1D[i]))
    plt.plot(X,np.fft.fftshift(plotdata1D[i]),label = f"fourier ved temp {data[i,1]}")

plt.title("singulær?")
#plt.legend()
plt.savefig("singulært_plot.png")

'''

plotdatamidt = plotdatawrapped[len(plotdatawrapped)//2]
X = np.arange(-len(plotdatawrapped)//2, len(plotdatawrapped)//2)

#cauchy function for fitting
def cauchy(x, x0, gamma, A):
    return A * (gamma**2 / ((x - x0)**2 + gamma**2))
# Fit the Cauchy function to the data
popt, pcov = curve_fit(cauchy, X, plotdatamidt, p0=[0, 10, max(plotdatamidt)], bounds=([-np.inf, 0, 0], [np.inf, np.inf, np.inf]))
x0_fit, gamma_fit, A_fit = popt
print(f"Fitted parameters: x0 = {x0_fit}, gamma = {gamma_fit}, A = {A_fit}")

#cauchy with offset
def cauchy_offset(x, x0, gamma, A, offset):
    return A * (gamma**2 / ((x - x0)**2 + gamma**2)) + offset
# Fit the Cauchy function with offset to the data
popt_offset, pcov_offset = curve_fit(cauchy_offset, X, plotdatamidt, p0=[0, 10, max(plotdatamidt), min(plotdatamidt)], bounds=([-np.inf, 0, 0, -np.inf], [np.inf, np.inf, np.inf, np.inf]))
x0_fit_offset, gamma_fit_offset, A_fit_offset, offset_fit = popt_offset
print(f"Fitted parameters with offset: x0 = {x0_fit_offset}, gamma = {gamma_fit_offset}, A = {A_fit_offset}, offset = {offset_fit}")

# Plot the data and the fitted curve
#plt.plot(X, plotdatamidt, 'o', label='Data')


plt.ylim(0, 750)
plt.xlabel("k")
plt.ylabel("Fourier Transform")
plt.title("Fourier Transform at B = 0.0, T = 2.8")
plt.plot(X,plotdatamidt, '.-', label="Simulation")

plt.plot(X, cauchy(X, x0_fit, gamma_fit, A_fit), '-', label='Cauchy fit')
plt.plot(X, cauchy_offset(X, x0_fit_offset, gamma_fit_offset, A_fit_offset, offset_fit), '-', label='Cauchy fit + offset', color='purple')


plt.grid()
plt.legend()
plt.savefig("fourierpeak.png", dpi=500)

'''








'''

# Plot heatmap: 
fig, ax = plt.subplots(figsize=(N, N))
im = ax.imshow(plotdatawrapped, origin='lower', aspect='auto', cmap=cmap)

# set ticks to the unique values
#ax.set_xticks(np.arange(len(B_unique)))
#ax.set_xticklabels([str(x) for x in B_unique], rotation=45)
#ax.set_yticks(np.arange(len(temperatures_unique)))
#ax.set_yticklabels([str(x) for x in temperatures_unique])

ax.set_xlabel(r"$k_x$")
ax.set_ylabel(r"$k_y$")
ax.set_title("2D Fourier Transform Heatmap")
fig.colorbar(im, ax=ax, label="norm of Fourier Transform")
plt.tight_layout()
plt.savefig('fourier_heatmap.png', dpi=300)
'''