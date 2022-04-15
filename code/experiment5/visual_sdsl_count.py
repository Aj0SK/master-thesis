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
            if len(initial_line_split) < 2 or initial_line_split[1] != "TC_ID":
                continue
            else:
                test_name = initial_line_split[3]
                used_structure = lines[i+1].split()[3]
                if "RRR" not in used_structure:
                    continue
                used_structure_split = used_structure.split("RRR")[1].split("_")
                test_block_size = int(used_structure_split[0])
                test_cutoff = test_block_size
                if len(used_structure_split) == 2:
                    test_cutoff = int(used_structure_split[1])

                test_rrr_size = 8*int(lines[i + 7].split()[3])
                test_memory = float(lines[i + 8].split()[3])/test_rrr_size
                test_time = float(lines[i + 16].split()[3])
                
            results[(test_name,
                    test_block_size, test_cutoff)] = (test_time, test_memory)
    return results


path = "../experiment3/sdsl-lite/benchmark/indexing_count/results/all.txt"

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

fig, axs = plt.subplots(len(test_cases))
slovak_title = 'Čas pre 10^7 operácií prístup, rank, select'
english_title = 'Access, rank and select query time as a function of block size '
fig.suptitle(english_title)

for i in range(len(test_cases)):
    axs[i].set_title(test_cases[i])
    #axs[i][1].set_title(test_cases[i] + " rank")
    #axs[i][2].set_title(test_cases[i] + " select")
    #axs[i][0].set_xticks([15, 31, 63, 127])
    #axs[i][1].set_xticks([15, 31, 63, 127])
    #axs[i][2].set_xticks([15, 31, 63, 127])

for file, block_size, cutoff in res.keys():
    time, memory = res[file, block_size, cutoff]
    print(file, block_size, cutoff)
    file_index = test_cases.index(file)
    m = 'o'
    marker_size = 30
    if cutoff != block_size:
        m = "${}$".format(cutoff)
        marker_size = 100
    
    color = block_size_color[block_size]
    print(memory)
    axs[file_index].scatter(memory, time, c=color, marker=m, s=marker_size)

fig.set_size_inches(7, 15)
plt.tight_layout()
plt.savefig("vysledky_sdsl.png")
plt.clf()