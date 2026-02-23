import numpy as np
import matplotlib.pyplot as plt
import matplotlib.cm as cm
import matplotlib.colors as mcolors

temperatures = np.loadtxt("output/temperatures.csv", delimiter=",", skiprows=0)
B = np.loadtxt("output/fields.csv", delimiter=",", skiprows=0)
data = np.loadtxt("output/magnetization.csv", delimiter=",", skiprows=1)


cmap = cm.viridis
norm = mcolors.Normalize(vmin=min(data[:,2]), vmax=max(data[:,2]))

# Build a 2D array of average magnetizations for (temperature x external field)
# First find unique temperature and field values while preserving file order
temp_vals, temp_idx = np.unique(temperatures, return_index=True)
temperatures_unique = temp_vals[np.argsort(temp_idx)]
field_vals, field_idx = np.unique(B, return_index=True)
B_unique = field_vals[np.argsort(field_idx)]

# Initialize heatmap array (rows=temperatures, cols=fields) with NaN
hm = np.full((len(temperatures_unique), len(B_unique)), np.nan)

for i, T in enumerate(temperatures_unique):
    for j, b in enumerate(B_unique):
        # use isclose for float comparisons
        sel = (np.isclose(data[:, 0], T)) & (np.isclose(data[:, 1], b))
        plotdata = data[sel]

        if plotdata.size == 0:
            continue

        steps = plotdata[:, 2]
        magnetization = plotdata[:, 3]

        # calculate the average magnetization for the last 80% of the steps
        last_80_percent = int(0.8 * len(magnetization))
        if last_80_percent <= 0:
            avg_magnetization = np.mean(magnetization)
        else:
            avg_magnetization = np.mean(magnetization[-last_80_percent:])

        hm[i, j] = avg_magnetization

print("heatmap shape:", hm.shape)
print(hm)

# Plot heatmap: rows -> temperatures, columns -> fields
fig, ax = plt.subplots(figsize=(6, 5))
im = ax.imshow(hm, origin='lower', aspect='auto', cmap=cmap)

# set ticks to the unique values
ax.set_xticks(np.arange(len(B_unique)))
ax.set_xticklabels([str(x) for x in B_unique], rotation=45)
ax.set_yticks(np.arange(len(temperatures_unique)))
ax.set_yticklabels([str(x) for x in temperatures_unique])

ax.set_xlabel('External Field')
ax.set_ylabel('Temperature')
ax.set_title('Average Magnetization Heatmap (last 80% of steps)')
fig.colorbar(im, ax=ax, label='Average |Magnetization|')
plt.tight_layout()
plt.savefig('B_heatmap.png', dpi=300)