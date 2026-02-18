import numpy as np
import matplotlib.pyplot as plt
import matplotlib.cm as cm
import matplotlib.colors as mcolors

temperatures = np.loadtxt("output/temperatures.csv", delimiter=",", skiprows=0)
data = np.loadtxt("output/magnetization.csv", delimiter=",", skiprows=1)

norm = mcolors.Normalize(vmin=min(temperatures), vmax=max(temperatures))
cmap = cm.viridis

fig0, ax0 = plt.subplots()

# Plot mean cluster size vs Monte Carlo steps for different temperatures
avg_mcss = []

for T in temperatures:
    plotdata = data[data[:, 0] == T]
    steps = plotdata[:, 1]
    mcs = plotdata[:, 3]  # mean cluster size

    color = cmap(norm(T))
    ax0.plot(steps, mcs, color=color, label=f"T={T}")

    # calculate the average magnetization for the last 80% of the steps
    last_80_percent = int(0.8 * len(mcs))
    avg_mcs = np.mean(mcs[-last_80_percent:])
    avg_mcss.append(avg_mcs)

ax0.set_xlabel("Monte Carlo Steps")
ax0.set_ylabel("Mean Cluster Size")
ax0.set_title(
    "Mean Cluster Size vs Monte Carlo Steps for Different Temperatures")

sm = cm.ScalarMappable(norm=norm, cmap=cmap)
sm.set_array([])
cbar = fig0.colorbar(sm, ax=ax0, label="Temperature")
cbar.ax.invert_yaxis()

plt.savefig("clusters_plot.png", dpi=300)


# Plot average mean cluster size vs temperature

fig1, ax1 = plt.subplots()

plt.figure()
ax1.plot(temperatures, avg_mcss, marker='o',
         color='blue', label='Avg Mean Cluster Size')
ax1.set_xlabel("Temperature")
ax1.set_ylabel("Average Mean Cluster Size (last 80% of steps)")
ax1.set_title("Average Mean Cluster Size vs Temperature")
ax1.grid()
fig1.savefig("avg_cms_vs_temperature.png", dpi=300)


# Ligger magnitisation oveni mean cluster size
ax2 = ax1.twinx()
avg_magnetizations = []

for T in temperatures:
    plotdata = data[data[:, 0] == T]
    steps = plotdata[:, 1]
    magnetization = plotdata[:, 2]

    # if magnetization[-1] < 0:
    #     magnetization *= -1

    color = cmap(norm(T))
    # ax2.plot(steps, magnetization, color=color, label=f"T={T}")

    # calculate the average magnetization for the last 80% of the steps
    last_80_percent = int(0.8 * len(magnetization))
    avg_magnetization = np.mean(abs(magnetization[-last_80_percent:]))
    avg_magnetizations.append(avg_magnetization)

scaled_magnetizations = np.array(
    avg_magnetizations) / np.max(avg_magnetizations) * np.max(avg_mcss)


ax2.plot(temperatures, scaled_magnetizations, marker='o',
         color='red', label='Scaled Avg Magnetization')
fig1.legend()
fig1.savefig("avg_mag&cms1.png", dpi=300)
