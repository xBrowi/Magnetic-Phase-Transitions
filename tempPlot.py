import numpy as np
import matplotlib.pyplot as plt
import matplotlib.cm as cm
import matplotlib.colors as mcolors
temperatures = np.loadtxt("temperatures.csv", delimiter=",", skiprows=0)
# emperatures = temperatures[temperatures < 2.5]  # Fjern eventuelle negative eller nul temperaturer
data = np.loadtxt("magnetization.csv", delimiter=",", skiprows=1)

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

    # calculate the average magnetization for the last 50% of the steps
    last_50_percent = int(0.5 * len(magnetization))
    avg_magnetization = np.mean(magnetization[-last_50_percent:])
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
plt.show()

plt.plot(temperatures, avg_magnetizations, marker='o')
plt.xlabel("Temperature")
plt.ylabel("Average Magnetization (last 50% of steps)")
plt.title("Average Magnetization vs Temperature")
plt.grid()
plt.savefig("avg_magnetization_vs_temperature.png", dpi=300)
plt.show()