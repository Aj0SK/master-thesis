from os import listdir
from os.path import isfile, join

import numpy as np
import matplotlib.pyplot as plt

plt.rcParams["figure.figsize"] = (10,12)
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
                #opt_lvl = lines[i+2].split()[3]
                #if opt_lvl != "SSE":
                #    continue
                test_name = initial_line_split[3]
                test_structure = lines[i+1].split()[3]
                test_rrr_size = 8*int(lines[i + 8].split()[3])
                test_memory = float(lines[i + 9].split()[3])/test_rrr_size
                test_time = float(lines[i + 12].split()[3])
                
            results[(test_name, test_structure)] = (test_time, test_memory)
    return results

map_name = {"FM_HUFF" : "bv", "FM_HUFF_RRR15": "rrr_15",
            "FM_HUFF_RRR31": "rrr_31",
            "FM_HUFF_RRR63": "rrr_63",
            "FM_HUFF_RRR127": "rrr_127"}

reimplemented = ["FM_HUFF_RRR31", "FM_HUFF_RRR63", "FM_HUFF_RRR127"]

old_version_path = "./res1-extract.txt"
new_version_path = "./res2-extract.txt"

res1 = return_results(old_version_path)
res2 = return_results(new_version_path)

res = [res1, res2]
datasets = []

for r in res:
    for (dataset_name, _) in r.keys():
        if dataset_name not in datasets:
            datasets.append(dataset_name)

fig, axs = plt.subplots(len(datasets))
pts = [set() for i in range(len(datasets))]
fig.suptitle('FM-index extract')

for r in range(len(res)):
    for dataset_name, structure in res[r].keys():
        time, memory = res[r][dataset_name, structure]
        if structure not in reimplemented and names[r] == "our":
            continue
        dataset_index = datasets.index(dataset_name)
        x = [memory]
        y = [time]
        axs[dataset_index].scatter(x, y, label = names[r] + "-" + map_name[structure], marker = markers[r])
        axs[dataset_index].legend(loc='center left', bbox_to_anchor=(1, 0.5))
        axs[dataset_index].set_title(dataset_name)
        for yy in y:
            pts[dataset_index].add(yy)

for i in range(len(datasets)):
    axs[i].set_yticks(list(pts[i]))
    axs[i].set_ylabel("time (ms)")
    axs[i].set_xlabel("bits per bit")

fig.tight_layout(pad=3.0)

plt.savefig("vysledky_sdsl_extract.png", bbox_inches='tight')
plt.clf()