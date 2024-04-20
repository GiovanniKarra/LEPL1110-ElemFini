import matplotlib.pyplot as plt
import numpy as np


data = np.genfromtxt("perf.csv", dtype=float, delimiter=",")
size = data[0][1:]
perf = data[1][1:]

ax = plt.figure()

plt.title("Analyse de performance selon la taille des éléments")

plt.xlabel("h")
plt.ylabel("temps d'execution [s]")

plt.gca().invert_xaxis()

plt.loglog(size, perf)
plt.loglog(size, 1e-4*size**(-3), linestyle="dashed")

plt.grid(which="major", linestyle="-")
plt.grid(which="minor", linestyle=":")

plt.legend(["Notre solveur", "$\mathcal{O}(m^3)$"])

# plt.show()
plt.savefig("rapport/images/perf.svg", format="svg")