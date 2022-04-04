import json
import re
import matplotlib.pyplot as plt
import numpy as np
from math import comb, log


def translate(str):
    preklady = [["Operation", "Operácia"], ["AccessPattern", "PrístupovýTyp"],
                ["Access", "Prístup"], ["ContinuousRandom", "NáhodnýLineárny"], ["Random", "Náhodný"]]
    for preklad in preklady:
        str = str.replace(preklad[0], preklad[1])
    return str


def cummulative_dist_f(x, n, p):
    sum = 0
    for i in range(x+1):
        sum += comb(n, i)*(p**i)*((1-p)**(n-i))
    return sum


def lower_bound(L, val):
    index = next(x[0] for x in enumerate(L) if x[1] < val)
    return index


def upper_bound(L, val):
    index = next(x[0] for x in enumerate(L) if x[1] > val)
    return index

# plt.rcParams["figure.figsize"] = (6,3)


block_sizes = [31, 63, 127]

fig, axs = plt.subplots(nrows=1, ncols=len(
    block_sizes), figsize=(9, 3), sharey=True)
fig.suptitle(
    "Ratio of space used by compressed form to raw form as a function of blocks class")

for (i, block_size) in enumerate(block_sizes):
    x = [i for i in range(block_size)]
    y = [(log(block_size, 2)+log(comb(block_size, i), 2))/log(2**block_size, 2)
         for i in range(block_size)]

    axs[i].axhline(y=1.0, color='0.9', linestyle='-')
    axs[i].plot(x, y, color="k")

    bad_start = upper_bound(y, 1.0)
    bad_end = bad_start+lower_bound(y[bad_start+1:], 1.0)

    start, end = axs[i].get_xlim()
    x_ticks = list(range(0, block_size, block_size//4))
    x_ticks = [i for i in x_ticks if i <
               x[bad_start] or i > x[bad_end]]
    x_ticks += [x[bad_start], x[bad_end]]
    x_ticks.sort()
    axs[i].xaxis.set_ticks(x_ticks)

    for j in range(len(x_ticks)):
        if y[x_ticks[j]] > 1.0:
            axs[i].get_xticklabels()[j].set_color("k")
            axs[i].get_xticklabels()[j].set_fontsize("x-large")
        else:
            axs[i].get_xticklabels()[j].set_color("0.5")
    axs[i].set_title(translate("b=" + str(block_size)))


fig.tight_layout()
plt.show()
# plt.savefig("binomial_dist.png", bbox_inches='tight')
# plt.clf()
