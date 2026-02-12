import numpy as np
import matplotlib.pyplot as plt
import matplotlib.cm as cm
import matplotlib.colors as mcolors

from scipy.stats import gaussian_kde
import seaborn as sns

temperatures = np.loadtxt("output/temperatures.csv", delimiter=",", skiprows=0)
# temperatures = temperatures[temperatures == 2.0]  # Fjern eventuelle negative eller nul temperaturer
data = np.loadtxt("output/magnetization.csv", delimiter=",", skiprows=1)

# Normalisering af temperaturintervallet
norm = mcolors.Normalize(vmin=min(temperatures), vmax=max(temperatures))
cmap = cm.viridis

fig, ax = plt.subplots()

avg_magnetizations = []

for T in temperatures:
    plotdata = data[data[:, 0] == T]
    steps = plotdata[:, 1]
    magnetization = plotdata[:, 2]

    # if magnetization[-1] < 0:
    #     magnetization *= -1

    color = cmap(norm(T))
    ax.plot(steps, magnetization, color=color, label=f"T={T}")

    # calculate the average magnetization for the last 80% of the steps
    last_80_percent = int(0.8 * len(magnetization))
    avg_magnetization = np.mean(abs(magnetization[-last_80_percent:]))
    avg_magnetizations.append(avg_magnetization)

ax.set_xlabel("Monte Carlo Steps")
ax.set_ylabel("Absolute Magnetization")
ax.set_title("Magnetization vs Monte Carlo Steps for Different Temperatures")
# ax.legend()

sm = cm.ScalarMappable(norm=norm, cmap=cmap)
sm.set_array([])
cbar = fig.colorbar(sm, ax=ax, label="Temperature")
cbar.ax.invert_yaxis()


plt.savefig("magnetization_plot.png", dpi=300)

plt.figure()

plt.plot(temperatures, avg_magnetizations, marker='o')
plt.xlabel("Temperature")
plt.ylabel("Average Magnetization (last 80% of steps)")
plt.title("Average Magnetization vs Temperature")
plt.grid()
plt.savefig("avg_magnetization_vs_temperature.png", dpi=300)
#plt.show()




magnetization_guesses = []

for t in temperatures:
    plotdata = abs(data[data[:, 0] == t])

    # only consider the last 80% of the steps for the histogram
    last_80_percent = int(0.8 * len(plotdata))

    plotdata = plotdata[-last_80_percent:]

    bins = np.linspace(0, 1, 200)  # Juster antallet af bins efter behov

    bins, edges = np.histogram(plotdata[:, 2], bins=100, density=True)
    bin_centers = 0.5 * (edges[1:] + edges[:-1])

    plt.plot(bin_centers, bins, label=f"Temperature={t}")

    magnetization_guess = bin_centers[np.argmax(bins)]
    magnetization_guesses.append(magnetization_guess)

    plt.axvline(x=magnetization_guess, color='k', linestyle='--')

plt.xlabel("Magnetization")
plt.ylabel("Density")
plt.title("Magnetization Distribution for Different Temperatures")
plt.grid()
plt.savefig("magnetization_distribution.png", dpi=300)
# plt.legend()
#plt.show()

plt.plot(temperatures, magnetization_guesses, marker='o')
plt.xlabel("Temperature")
plt.ylabel("Most Probable Magnetization")
plt.title("Most Probable Magnetization vs Temperature")
plt.grid()
plt.savefig("most_probable_magnetization_vs_temperature.png", dpi=300)
#plt.show()


# even more signal processing - kde

kdemaxes = []

for t in temperatures:
    plotdata = abs(data[data[:, 0] == t])

    # only consider the last 80% of the steps for the histogram
    last_80_percent = int(0.8 * len(plotdata))

    plotdata = plotdata[-last_80_percent:]

    kde = gaussian_kde(plotdata[:, 2])
    x = np.linspace(0, 1, 1000)
    kde_values = kde(x)
    plt.plot(x, kde_values, label=f"Temperature={t}")

    kdemaxes.append(x[np.argmax(kde_values)])

plt.xlabel("Magnetization")
plt.ylabel("Density")
plt.title("Magnetization KDE for Different Temperatures")
plt.grid()
plt.savefig("magnetization_kde.png", dpi=300)

#peak:
for x in kdemaxes:
    plt.axvline(x=x, color='k', linestyle='--')

#plt.show()

plt.plot(temperatures, kdemaxes, marker='o')
plt.xlabel("Temperature")
plt.ylabel("Most Probable Magnetization (KDE)")
plt.title("Most Probable Magnetization (KDE) vs Temperature")
plt.grid()
plt.savefig("most_probable_magnetization_kde_vs_temperature.png", dpi=300)
#plt.show()


# kdeplot

# for t in temperatures:
#     plotdata = abs(data[data[:, 0] == t])

#     # only consider the last 80% of the steps for the histogram
#     last_80_percent = int(0.8 * len(plotdata))

#     plotdata = plotdata[-last_80_percent:]

#     sns.kdeplot(plotdata[:, 2], label=f"Temperature={t}")

# plt.xlabel("Magnetization")
# plt.ylabel("Density")
# plt.title("Magnetization KDE for Different Temperatures")
# plt.grid()
# plt.savefig("magnetization_kde.png", dpi=300)
# # plt.legend()
# #plt.show()