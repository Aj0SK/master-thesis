from os import listdir
from os.path import isfile, join

import matplotlib.pyplot as plt
import numpy as np


def translate(s):
    if s == "ENGLISH":
        return "Anglický text"
    elif s == "SOURCES":
        return "Zdrojové kódy"
    return s


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
                used_structure_split = used_structure.split("RRR")[
                    1].split("_")
                test_block_size = int(used_structure_split[0])
                test_cutoff = test_block_size
                if len(used_structure_split) == 2:
                    test_cutoff = int(used_structure_split[1])

                test_text_size = int(lines[i + 7].split()[3])
                test_index_size = 8*int(lines[i + 8].split()[3])
                test_memory = test_index_size/test_text_size
                test_time = float(lines[i + 17].split()[3])*1000.0

            results[(test_name,
                    test_block_size, test_cutoff)] = (test_time, test_memory)
    return results


# DNA
# SOURCES
# ENGLISH
DATASET = "DNA"

path = "./res-count.txt"

colors = ['r', 'g', 'b', 'darkorange']
block_size_color = {15: 'r', 31: 'g', 63: 'b', 127: 'darkorange'}

res = return_results(path)

test_cases = set()
block_sizes = set()

for file, block_size, cutoff in res.keys():
    test_cases.add(file)
    block_sizes.add(block_size)

test_cases = list(test_cases)
block_sizes = list(block_sizes)

test_cases.sort()

rows, cols = 1, 1

fig, axs = plt.subplots(rows, cols, figsize=(8, 6))
fig.suptitle('FM-index spočítaj pre hybridnú implementáciu', fontsize=20)

for i in range(len(test_cases)):
    if test_cases[i] == DATASET:
        axs.set_title(test_cases[i], fontsize=16)

labels = set()

allowedCutoffs = {15: [15], 31: [15, 31], 63: [15, 63], 127: [31, 127]}

for file, block_size, cutoff in res.keys():
    time, memory = res[file, block_size, cutoff]
    if cutoff not in allowedCutoffs[block_size]:
        continue
    if file != DATASET:
        continue

    file_index = test_cases.index(file)
    m = 'o'
    marker_size = 140
    l = "orig-" + str(block_size)
    if cutoff != block_size:
        m = "x"
        l = "hybrid-" + str(block_size) + "(" + str(cutoff) + ")"

    color = block_size_color[block_size]

    labels.add(l)
    axs.scatter([memory], [time], c=color, marker=m, s=marker_size, label=l)


axs.set_ylabel("µs na symbol vzorky", fontsize=16)
axs.set_xlabel("bity na znak textu", fontsize=16)

labels_handles = {
    label: handle for ax in fig.axes for handle, label in zip(*ax.get_legend_handles_labels())
}

fig.legend(
    labels_handles.values(),
    labels_handles.keys(),
    loc="upper center",
    bbox_to_anchor=(0.5, 0),
    bbox_transform=plt.gcf().transFigure,
    ncol=len(list(labels))//2,
    handletextpad=0.01,
    labelspacing=0.0,
    borderpad=0.1,
    columnspacing=1.0,
    prop={'size': 15}
)

fig.tight_layout(pad=1.0)

plt.savefig(f"vysledky_sdsl_hybrid_count_{DATASET}.png", bbox_inches='tight')
plt.clf()
