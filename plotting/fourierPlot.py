import numpy as np
import matplotlib.pyplot as plt
import matplotlib.cm as cm
import matplotlib.colors as mcolors


data = np.loadtxt("output/fourier_output.csv", delimiter=" ", skiprows=0)
data2D = np.array(np.split(data, len(data)//32))
print(data,data2D)

norms = np.sqrt(data[:, 0]**2 + data[:, 1]**2)
norms2D = np.sqrt(data2D[:,:,0]**2 + data2D[:,:,1]**2)
X = np.linspace(0, 1, len(norms))
X2D = np.linspace(0, 1, len(norms2D))
print(X,"\n", norms)


plt.plot(X,data[:,0], label="Real part")
#plt.plot(X,data[1:,1], label="Imaginary part")
plt.plot(X,norms, label="Norm")

plt.xlabel("k (genereliseret frekvens)")
plt.ylabel("Fourier Transform")
plt.title("1D Fourier Transform")
plt.legend()
plt.savefig("fourier_plot_1D.png", dpi=500)
plt.close()



norm = mcolors.Normalize(vmin=0, vmax=len(norms2D))
cmap = cm.viridis

for i in range(len(norms2D)):
    plt.plot(X2D, norms2D[i,:],color = cmap(norm(i)), label=f"Row {i}")



plt.title("2D reciprocal space?")
#plt.legend()
plt.savefig("fourier_plot.png", dpi=500)