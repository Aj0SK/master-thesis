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

                test_text_size = int(lines[i + 7].split()[3])
                test_index_size = 8*int(lines[i + 8].split()[3])
                test_memory = test_index_size/test_text_size
                test_time = float(lines[i + 17].split()[3])*1000.0
                
            results[(test_name,
                    test_block_size, test_cutoff)] = (test_time, test_memory)
    return results


path = "./res-count.txt"

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

rows, cols = 2, 2

fig, axs = plt.subplots(rows, cols, figsize=(11,6))
fig.suptitle('FM-index count')

if rows*cols != len(test_cases):
    print("Not enough of space in graph.")
    exit(1)

for i in range(len(test_cases)):
    axs[i//cols][i%cols].set_title(test_cases[i])

labels = set()

for file, block_size, cutoff in res.keys():
    time, memory = res[file, block_size, cutoff]
    file_index = test_cases.index(file)
    m = 'o'
    marker_size = 20
    if cutoff != block_size:
        m = fr"${cutoff}$"
        marker_size = 100
        if len(m) == 3:
            marker_size = 45
    
    row = file_index//rows
    col = file_index%cols
    color = block_size_color[block_size]
    l = '_nolegend_'
    if cutoff == block_size:
        l = str(block_size)
    labels.add(l)
    axs[row][col].scatter([memory], [time], c=color, marker=m, s=marker_size, label=l)

for i in range(rows):
    axs[i][0].set_ylabel("Time in µsec per pattern symbol")

for i in range(cols):
    axs[-1][i].set_xlabel("bits per symbol")

fig.subplots_adjust(bottom=0.3, wspace=0.33)

labels_handles = {
  label: handle for ax in fig.axes for handle, label in zip(*ax.get_legend_handles_labels())
}

fig.legend(
  labels_handles.values(),
  labels_handles.keys(),
  loc="upper center",
  bbox_to_anchor=(0.5, 0.1),
  bbox_transform=plt.gcf().transFigure,
  ncol=len(list(labels)),
  handletextpad=0.01
)

fig.tight_layout(pad=3.0)

plt.savefig("vysledky_sdsl_hybrid_count.png")
plt.clf()