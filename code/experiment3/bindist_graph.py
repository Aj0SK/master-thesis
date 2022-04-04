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
        sum += comb(n, i)*(p**i)*((1-p)**(n-i))
    return sum


def upper_bound(L, val):
    index = next(x[0] for x in enumerate(L) if x[1] > val)
    return index

# plt.rcParams["figure.figsize"] = (6,3)


block_sizes = [31, 63, 127]
percentage = 0.05

fig, axs = plt.subplots(nrows=1, ncols=len(
    block_sizes), figsize=(9, 3), sharey=True)
fig.suptitle(
    "Cumulative probability of block classes for various block sizes ({}% of ones)".format(int(100*percentage)))

for (i, block_size) in enumerate(block_sizes):
    x = [i for i in range(block_size)]
    y = [cummulative_dist_f(i, block_size, percentage)
         for i in range(block_size)]
    threshold_index = upper_bound(y, 0.99)
    threshold_value = x[threshold_index]

    axs[i].plot(x, y, color="k")

    start, end = axs[i].get_xlim()
    x_ticks = list(range(0, block_size, block_size//5))
    x_ticks += [threshold_value]
    x_ticks.sort()
    axs[i].xaxis.set_ticks(x_ticks)
    special_tick_index = upper_bound(
        x_ticks, threshold_value)

    for j in range(len(x_ticks)):
        if j is special_tick_index:
            axs[i].get_xticklabels()[j].set_color("k")
        else:
            axs[i].get_xticklabels()[j].set_color("0.5")

    axs[i].get_xticklabels()[special_tick_index].set_fontsize("x-large")
    axs[i].set_title(translate("b=" + str(block_size)))


fig.tight_layout()
plt.show()
# plt.savefig("binomial_dist.png", bbox_inches='tight')
# plt.clf()
