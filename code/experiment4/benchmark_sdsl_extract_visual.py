from os import listdir
from os.path import isfile, join

import numpy as np
import matplotlib.pyplot as plt

plt.rcParams["figure.figsize"] = (10,12)
markers = ["s", "o"]

def return_results(path):
    results = [{}]
    with open(path, "r") as f:
        lines = f.readlines()
        for i in range(len(lines)):
            if lines[i] == "\n":
                results.append({})
                continue
            line_split = lines[i].split()
            if len(line_split) < 4:
                continue
            results[-1][line_split[1]] = line_split[3]
    results.pop()
    return results

contains = [["IDX_ID", "FM_HUFF", "FM_HUFF_RRR15", "FM_HUFF_RRR63", "FM_HUFF_RRR127"], ["SAMPLE_ID", "64x64"]]#, ["COMPILE_ID", "SSE"]]

old_version_path = "./res1-extract.txt"
new_version_path = "./res2-extract.txt"

res1 = return_results(old_version_path)
res2 = return_results(new_version_path)

res = [res1, res2]
datasets = []

for r in res:
    for k in r:
        dataset_name = k["test_case"]
        if dataset_name not in datasets:
            datasets.append(dataset_name)

fig, axs = plt.subplots(len(datasets))
pts = [set() for i in range(len(datasets))]
fig.suptitle('Vertically stacked subplots')

for r in range(len(res)):
    for k in res[r]:
        to_skip = False
        for filter in contains:
            if k[filter[0]] not in filter[1:]:
                to_skip = True
        if to_skip:
            continue
        dataset_name = k["test_case"]
        type = k["IDX_ID"]
        size_in_bytes = k["Index_size_in_bytes"]
        text_size = k["text_size"]
        dataset_index = datasets.index(dataset_name)
        x = [float(size_in_bytes)/float(text_size)]
        y = [float(k["Extract_time_in_sec"])]
        axs[dataset_index].scatter(x, y, label = str(r) + "-" + str(type), marker = markers[r])
        axs[dataset_index].legend(loc='center left', bbox_to_anchor=(1, 0.5))
        axs[dataset_index].set_title(dataset_name)
        for yy in y:
            pts[dataset_index].add(yy)

for i in range(len(datasets)):
    axs[i].set_yticks(list(pts[i]))
    axs[i].set_ylabel("čas (s)")
    axs[i].set_xlabel("bitov na bit")

fig.tight_layout(pad=3.0)

plt.savefig("vysledky_sdsl_extract.png", bbox_inches='tight')
plt.clf()



