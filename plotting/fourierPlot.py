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
plt.savefig("fourier_plot_2D.png", dpi=500)
plt.close()



# Plot heatmap: rows -> temperatures, columns -> fields
fig, ax = plt.subplots(figsize=(32, 32))
im = ax.imshow(norms2D, origin='lower', aspect='auto', cmap=cmap)

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