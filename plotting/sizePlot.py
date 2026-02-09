import numpy as np
import matplotlib.pyplot as plt
import matplotlib.cm as cm
import matplotlib.colors as mcolors
from scipy.optimize import curve_fit


sizes = np.loadtxt("sizes.csv", delimiter=",", skiprows=0)
#sizes = sizes[sizes >100]
data = np.loadtxt("magnetizationSize.csv", delimiter=",", skiprows=1)
data = np.abs(data)  # Tag den absolutte værdi af magnetiseringen

# Normalisering af temperaturintervallet
'''
norm = mcolors.Normalize(vmin=min(sizes), vmax=max(sizes))
cmap = cm.viridis

fig, ax = plt.subplots()

avg_magnetizations = []

for s in sizes:
    plotdata = data[data[:, 0] == s]
    steps = plotdata[:, 1]
    magnetization = plotdata[:, 2]

    if magnetization[-1] < 0:
        magnetization *= -1

    color = cmap(norm(s))
    ax.plot(steps, magnetization, color=color, label=f"Size={s}")

    # calculate the average magnetization for the last 50% of the steps
    last_50_percent = int(0.5 * len(magnetization))
    avg_magnetization = np.mean(abs(magnetization[-last_50_percent:]))
    avg_magnetizations.append(avg_magnetization)

ax.set_xlabel("Monte Carlo Steps")
ax.set_ylabel("Absolute Magnetization")
ax.set_title("Magnetization vs Monte Carlo Steps for Different Sizes")
# ax.legend()

sm = cm.ScalarMappable(norm=norm, cmap=cmap)
sm.set_array([])
cbar = fig.colorbar(sm, ax=ax, label="Size")
cbar.ax.invert_yaxis()


plt.savefig("magnetization_plot.png", dpi=300)
plt.show()

plt.plot(sizes, avg_magnetizations, marker='o')
plt.xlabel("Size")
plt.ylabel("Average Magnetization (last 50% of steps)")
plt.title("Average Magnetization vs Size")
plt.grid()
plt.savefig("avg_magnetization_vs_size.png", dpi=300)
plt.show()
'''



#signal processing
magnetization_guesses = []

for s in sizes:
    #bins_edges = np.linspace(0, 1, int(s/2))  # Juster antallet af bins efter behov
    bins, edges = np.histogram(data[:, 2][data[:, 0] == s], bins=100, density=True)
    bin_centers = 0.5 * (edges[1:] + edges[:-1])
    plt.plot(bin_centers, bins, label=f"Size={s}")
    magnetization_guess = bin_centers[np.argmax(bins)]
    magnetization_guesses.append(magnetization_guess)
    plt.axvline(x=magnetization_guess, color='k', linestyle='--')
    plt.legend()

plt.xlabel("Magnetization")
plt.ylabel("Frequency")
plt.title("Histogram of Magnetization Values")
plt.grid()
plt.show()

plt.plot(sizes, magnetization_guesses, marker='o')
plt.xlabel("Size")
plt.ylabel("Magnetization Guess")
plt.title("Magnetization Guess vs Size")
plt.grid()
plt.show()


#målet er det her plot for 2 variable, temperatur og sizes
plt.plot(1/sizes, magnetization_guesses, marker='o')
plt.xlabel("1/Size")
plt.ylabel("Magnetization Guess")
plt.title("Magnetization Guess vs 1/Size")
plt.xlim(0, max(1/sizes)*1.1)  # Juster x-aksens grænser efter behov
plt.grid()

# fit a linear function to the data
def linear_func(x, a, b):
    return a * x + b

popt, pcov = curve_fit(linear_func, 1/sizes, magnetization_guesses)
a, b = popt
x_fit = np.linspace(0, max(1/sizes)*1.1, 100)
y_fit = linear_func(x_fit, a, b)
plt.plot(x_fit, y_fit, color='red', linestyle='--', label=f"Fit: y = {a:.3f}x + {b:.3f}")

plt.axhline(y=b, color='green', linestyle='--', label=f"Extrapolated Magnetization at Infinite Size: {b:.3f}")

plt.legend()

plt.show()

print(magnetization_guesses)