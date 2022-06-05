import json
import re
import matplotlib.pyplot as plt
from math import log2

def entropy(p):
    q = (1.0-p)
    return p*log2(1.0/p) + q*log2(1.0/q)

def translate(str):
    preklady = [["Operation", "Operácia"], ["AccessPattern", "PrístupovýTyp"],
                ["Random", "Náhodný"]]
    for preklad in preklady:
        str = str.replace(preklad[0], preklad[1])
    return str

usedCutoffs = {}

class BenchRes:
    def __init__(self, data):
        struct_name = data["name"]
        split_by = r"[,<>]"
        struct_name_split = re.split(split_by, struct_name)
        struct_name_split = [i.strip()
                             for i in struct_name_split if i != '']
        print(struct_name_split)
        self.oper = struct_name_split[1]
        self.access_pat = struct_name_split[2]
        self.density = struct_name_split[4]
        self.block_size = int(struct_name_split[5])
        self.cutoff, self.is_hybrid = -1, False
        if len(struct_name_split) > 7:
            self.cutoff = int(struct_name_split[7])
            self.is_hybrid = True
            prev = set(usedCutoffs.get(self.block_size, []))
            prev.add(self.cutoff)
            usedCutoffs[self.block_size] = list(prev)
        self.time = float(data["cpu_time"])
        self.space = float(data["Space"])

file_path = "special.txt"

NUM_OF_QUERIES = 10000.0
ONES_PERCENTAGE = 5
results = []
markers = ["x", "s"]
colors = ['r', 'g', 'b', 'darkorange']

with open(file_path, "r") as f:
    data = json.load(f)
    results = [BenchRes(i) for i in data["benchmarks"]]

print(usedCutoffs)

# Access
# Rank
# Select
OPERATION = "Select"

fig, axs = plt.subplots(nrows=1, ncols=1, figsize=(10, 5))
fig.suptitle(
    f"{OPERATION} na postupnosti dĺžky 2^25 ({ONES_PERCENTAGE}% jednotiek) - entropia {entropy(ONES_PERCENTAGE/100):.2f}",
    fontsize=20)

isHybrid = [True, False]
implNames = ["hybrid", "orig"]

labels = set()

for result_index in range(2):
    for (index1, access_pattern) in enumerate(["AccessPattern::Random"]):
        for (index2, oper) in enumerate(["Operation::Access", "Operation::Rank", "Operation::Select"]):
            if oper != "Operation::" + OPERATION:
                continue

            for (index3, block_size) in enumerate([15, 31, 63, 127]):
                x = [res.space for res in results if (res.oper, res.access_pat, res.block_size, res.is_hybrid) == (
                    oper, access_pattern, block_size, isHybrid[result_index])]
                y = [res.time/NUM_OF_QUERIES for res in results if (res.oper, res.access_pat, res.block_size, res.is_hybrid) == (
                    oper, access_pattern, block_size, isHybrid[result_index])]
                if x:
                    l = implNames[result_index] + "-" + str(block_size)
                    if result_index == 0:
                        l = l + "(" + str(usedCutoffs[block_size][0]) + ")"
                    labels.add(l)
                    axs.scatter(x, y, label=l, marker=markers[result_index], s=120, c=colors[index3])

            axs.set_xlabel("bitov na bit", fontsize=16)

            if result_index == 1:
                x = [res.space for res in results if (
                    res.oper, res.access_pat, res.block_size) == (oper, access_pattern, 0)]
                y = [res.time/NUM_OF_QUERIES for res in results if (
                    res.oper, res.access_pat, res.block_size) == (oper, access_pattern, 0)]
                l = "riedke-pole"
                labels.add(l)
                axs.scatter(x, y, label=l, marker='*', s=120, c="0.0")

#axs[-1].legend(loc='center left', bbox_to_anchor=(1, 0.5))
axs.set_ylabel("čas na otázku (ns)", fontsize=16)

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
  columnspacing = 1.0,
  prop={'size': 20}
)

fig.tight_layout(pad=3.0)

plt.savefig("vysledky_hybrid_artif_select.png", bbox_inches='tight')
plt.clf()