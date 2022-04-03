import json
import re
import matplotlib.pyplot as plt
import numpy as np
from math import comb

def translate(str):
    preklady = [["Operation", "Operácia"], ["AccessPattern", "PrístupovýTyp"],
    ["Access", "Prístup"], ["ContinuousRandom", "NáhodnýLineárny"], ["Random", "Náhodný"]]
    for preklad in preklady:
        str = str.replace(preklad[0], preklad[1])
    return str

def cummulative_dist_f(x, n, p):
	sum = 0
	for i in range(x+1):
		sum += comb(n,i)*(p**i)*((1-p)**(n-i))
	return sum

# plt.rcParams["figure.figsize"] = (6,3)

block_sizes = [31, 63, 127]
percentage = 0.05

fig, axs = plt.subplots(nrows=1, ncols=len(block_sizes))
fig.suptitle('Binomické rozdelenie')

for (i, block_size) in enumerate(block_sizes):
	#axs[i].set_xlabel("bitov na bit")
	#axs[i].set_ylabel("čas (ns)")
	x = [i for i in range(block_size)]
	y = [cummulative_dist_f(i, block_size, percentage) for i in range(block_size)]
	axs[i].plot(x, y)
	start, end = axs[i].get_xlim()
	axs[i].xaxis.set_ticks(np.arange(0, block_size, 10.0))
	#axs[i].legend(loc='center left', bbox_to_anchor=(1, 0.5))
	axs[i].set_title(translate(str(block_size)))


#fig.tight_layout(pad=3.0)

plt.savefig("binomial_dist.png", bbox_inches='tight')
plt.clf()
