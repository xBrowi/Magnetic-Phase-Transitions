import numpy as np
import matplotlib.pyplot as plt


data = np.loadtxt("output/fourier_output.csv", delimiter=" ", skiprows=0)
norms = np.sqrt(data[:, 0]**2 + data[:, 1]**2)
X = np.linspace(0, 1, len(norms))

plt.plot(X, norms)