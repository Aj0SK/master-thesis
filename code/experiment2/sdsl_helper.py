from os import listdir
from os.path import isfile, join

import matplotlib.pyplot as plt
import numpy as np


def return_results(path):
    results = {}
    with open(path, "r") as f:
        lines = f.readlines()
        for i in range(0, len(lines), 19):
            initial_line_split = lines[i].split()
            if initial_line_split[1] != "TC_ID":
                exit(1)
            else:
                test_name = initial_line_split[3]
                test_block_size = int(lines[i + 8].split()[3])
                # time in nanosecond per operation
                test_access = float(lines[i + 13].split()[3])
                test_rank = float(lines[i + 15].split()[3])
                test_select = float(lines[i + 17].split()[3])

            results[test_name,
                    test_block_size] = (test_access, test_rank, test_select)
    return results


old_version_path = "./res1.txt"
new_version_path = "./res2.txt"

colors = ['r', 'g', 'b', 'darkorange']

res1 = return_results(old_version_path)
res2 = return_results(new_version_path)

test_cases = set()
block_sizes = set()

for file, block_size in res1.keys():
    test_cases.add(file)
    block_sizes.add(block_size)

for file, block_size in res2.keys():
    test_cases.add(file)
    block_sizes.add(block_size)

test_cases = list(test_cases)
block_sizes = list(block_sizes)

fig, axs = plt.subplots(len(test_cases), 3)
slovak_title = 'Čas pre 10^7 operácií prístup, rank, select'
english_title = 'Access, rank and select query time as a function of block length'
fig.suptitle(english_title)

for i in range(len(test_cases)):
    axs[i][0].set_title(test_cases[i] + " access")
    axs[i][1].set_title(test_cases[i] + " rank")
    axs[i][2].set_title(test_cases[i] + " select")
    axs[i][0].set_xticks([15, 31, 63, 127])
    axs[i][1].set_xticks([15, 31, 63, 127])
    axs[i][2].set_xticks([15, 31, 63, 127])

for i in range(len(test_cases)-1):
    axs[i][0].set_xticklabels([])
    axs[i][1].set_xticklabels([])
    axs[i][2].set_xticklabels([])

for file, block_size in res1.keys():
    access, rank, select = res1[file, block_size]
    file_index = test_cases.index(file)
    axs[file_index][0].scatter(block_size, access, c='r', marker='o')
    axs[file_index][1].scatter(block_size, rank, c='g', marker='o')
    axs[file_index][2].scatter(block_size, select, c='b', marker='o')

for file, block_size in res2.keys():
    if block_size == 15:
        continue
    access, rank, select = res2[file, block_size]
    file_index = test_cases.index(file)
    axs[file_index][0].scatter(block_size, access, c='r', marker='+')
    axs[file_index][1].scatter(block_size, rank, c='g', marker='+')
    axs[file_index][2].scatter(block_size, select, c='b', marker='+')

for i in range(len(test_cases)):
    axs[i][0].set_ylabel("time (ns)")
    #axs[i][0].set_xlabel("veľkosť bloku")
    #axs[i][1].set_xlabel("veľkosť bloku")

axs[-1][0].set_xlabel("block_size")
axs[-1][1].set_xlabel("block_size")
axs[-1][2].set_xlabel("block_size")

fig.set_size_inches(8, 10)
plt.tight_layout()
plt.savefig("benchmark_sdsl_new_method.png")
plt.clf()