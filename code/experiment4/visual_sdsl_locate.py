from os import listdir
from os.path import isfile, join

import numpy as np
import matplotlib.pyplot as plt

markers = ["s", "x"]
names = ["sdsl", "our"]

def return_results(path):
    results = {}
    with open(path, "r") as f:
        lines = f.readlines()
        for i in range(len(lines)):
            initial_line_split = lines[i].split()
            if len(initial_line_split) < 2 or initial_line_split[1] != "TC_ID":
                continue
            else:
                if lines[i+3].split()[3] != "16x16":
                    continue
                test_name = initial_line_split[3]
                test_structure = lines[i+1].split()[3]
                test_text_size = int(lines[i + 8].split()[3])
                test_index_size = 8*float(lines[i + 9].split()[3])
                test_memory = test_index_size/test_text_size
                test_time = float(lines[i + 16].split()[3])*1000.0
                
            results[(test_name, test_structure)] = (test_time, test_memory)
    return results

map_name = {"FM_HUFF" : "bv", "FM_HUFF_RRR15": "rrr_15",
            "FM_HUFF_RRR31": "rrr_31",
            "FM_HUFF_RRR63": "rrr_63",
            "FM_HUFF_RRR127": "rrr_127", "CSA_SADA": "sparse"}

reimplemented = ["FM_HUFF_RRR31", "FM_HUFF_RRR63", "FM_HUFF_RRR127"]

old_version_path = "./res1-locate.txt"
new_version_path = "./res2-locate.txt"

rows, cols = 2, 2

res1 = return_results(old_version_path)
res2 = return_results(new_version_path)

res = [res1, res2]
datasets = []

for r in res:
    for (dataset_name, _) in r.keys():
        if dataset_name not in datasets:
            datasets.append(dataset_name)

if rows*cols != len(datasets):
    print("Not enough of space in graph.")
    exit(1)

fig, axs = plt.subplots(rows, cols, figsize=(10,6))
pts = [set() for i in range(len(datasets))]
fig.suptitle('FM-index locate')

labels = set()

for r in range(len(res)):
    for dataset_name, structure in res[r].keys():
        time, memory = res[r][dataset_name, structure]
        if structure not in reimplemented and names[r] == "our":
            continue
        dataset_index = datasets.index(dataset_name)
        x = [memory]
        y = [time]
        row = dataset_index//rows
        col = dataset_index%cols
        l = names[r] + "-" + map_name[structure]
        labels.add(l)
        axs[row][col].scatter(x, y, label = l, marker = markers[r])
        axs[row][col].set_title(dataset_name)
        for yy in y:
            pts[dataset_index].add(yy)

for i in range(rows):
    axs[i][0].set_ylabel("Time per occurrence")

for i in range(cols):
    axs[-1][i].set_xlabel("bits per symbol")

for i in range(rows):
    for j in range(cols):
        axs[i][j].set_yticks(list(pts[i*rows+j]))

fig.subplots_adjust(bottom=0.3, wspace=0.33)

labels_handles = {
  label: handle for ax in fig.axes for handle, label in zip(*ax.get_legend_handles_labels())
}

fig.legend(
  labels_handles.values(),
  labels_handles.keys(),
  loc="upper center",
  bbox_to_anchor=(0.5, 0),
  bbox_transform=plt.gcf().transFigure,
  ncol=len(list(labels))
)

fig.tight_layout(pad=1.0)

plt.savefig("vysledky_sdsl_locate.png", bbox_inches='tight')
plt.clf()