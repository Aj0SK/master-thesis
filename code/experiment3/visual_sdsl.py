from os import listdir
from os.path import isfile, join

import matplotlib.pyplot as plt
import numpy as np


def return_results(path):
    results = {}
    with open(path, "r") as f:
        lines = f.readlines()
        for i in range(len(lines)):
            initial_line_split = lines[i].split()
            if initial_line_split[1] != "TC_ID":
                continue
            else:
                test_name = initial_line_split[3]
                test_rrr_size = int(lines[i + 6].split()[3])
                test_block_size = int(lines[i + 8+1].split()[3])
                test_cutoff = int(lines[i + 8 + 2].split()[3])
                test_byte_size = float(lines[i + 11 + 1].split()[3]);
                test_memory = 8*test_byte_size/test_rrr_size
                test_access = float(lines[i + 13 + 2].split()[3])
                test_rank = float(lines[i + 15 + 2].split()[3])
                test_select = float(lines[i + 17 + 2].split()[3])

            results[(test_name,
                    test_block_size, test_cutoff)] = (test_access, test_rank, test_select, test_memory)
    return results


path = "./sdsl-lite/benchmark/rrr_vector/results/all.txt"

colors = ['r', 'g', 'b', 'darkorange']
block_size_color = {15: 'r', 31: 'g', 63: 'b', 127: 'darkorange' }

res = return_results(path)

test_cases = set()
block_sizes = set()

for file, block_size, cutoff in res.keys():
    test_cases.add(file)
    block_sizes.add(block_size)

test_cases = list(test_cases)
block_sizes = list(block_sizes)

fig, axs = plt.subplots(len(test_cases), 3)
slovak_title = 'Čas pre 10^7 operácií prístup, rank, select'
english_title = 'Access, rank and select query time as a function of space for hybrid implementation'
fig.suptitle(english_title, fontsize=20)

labels = set()

allowedCutoffs = {15: [15], 31: [7, 31], 63: [7, 63], 127: [15, 127]}

for i in range(len(test_cases)):
    axs[i][0].set_title(test_cases[i] + " access", fontsize=16)
    axs[i][1].set_title(test_cases[i] + " rank", fontsize=16)
    axs[i][2].set_title(test_cases[i] + " select", fontsize=16)
    #axs[i][0].set_xticks([15, 31, 63, 127])
    #axs[i][1].set_xticks([15, 31, 63, 127])    
    #axs[i][2].set_xticks([15, 31, 63, 127])

for file, block_size, cutoff in res.keys():
    access, rank, select, memory = res[file, block_size, cutoff]
    print(file, block_size, cutoff)
    if cutoff not in allowedCutoffs[block_size]:
        continue
    file_index = test_cases.index(file)
    m = 'o'
    marker_size = 75
    l = "orig-" + str(block_size)
    if cutoff != block_size:
        m = "x"
        l = "hybrid-" + str(block_size) + "(" + str(cutoff) + ")"
    labels.add(l)
    color = block_size_color[block_size]
    
    print(memory)
    axs[file_index][0].scatter(memory, access, c=color, marker=m, s=marker_size, label=l)
    axs[file_index][1].scatter(memory, rank, c=color, marker=m, s=marker_size, label=l)
    axs[file_index][2].scatter(memory, select, c=color, marker=m, s=marker_size, label=l)

for i in range(len(test_cases)):
    axs[i][0].set_ylabel("time (ns)", fontsize=12)
    #axs[i][0].set_xlabel("veľkosť bloku")
    #axs[i][1].set_xlabel("veľkosť bloku")

axs[-1][0].set_xlabel("bits per bit", fontsize=12)
axs[-1][1].set_xlabel("bits per bit", fontsize=12)
axs[-1][2].set_xlabel("bits per bit", fontsize=12)

#fig.subplots_adjust(bottom=0.3, wspace=0.33)

labels_handles = {
  label: handle for ax in fig.axes for handle, label in zip(*ax.get_legend_handles_labels())
}

axs[1][1].legend(
  labels_handles.values(),
  labels_handles.keys(),
  loc="upper center",
  bbox_to_anchor=(0.5, -0.2),
  ncol=len(list(labels)),
  handletextpad=0.01,
  labelspacing=0.0,
  borderpad=0.1,
  columnspacing = 1.0,
  prop={'size': 13}
)

fig.tight_layout(pad=1.0)

fig.set_size_inches(15, 8)
plt.savefig("vysledky_sdsl_hybrid.png")
plt.clf()