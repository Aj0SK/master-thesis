import json
import re
import matplotlib.pyplot as plt


def translate(str):
    preklady = [["Operation", "Operácia"], ["AccessPattern", "PrístupovýTyp"],
                ["Random", "Náhodný"]]
    for preklad in preklady:
        str = str.replace(preklad[0], preklad[1])
    return str


class BenchRes:
    def __init__(self, data):
        struct_name = data["name"]
        split_by = r"[,<>]"
        struct_name_split = re.split(split_by, struct_name)
        struct_name_split = [i.strip()
                             for i in struct_name_split if i != '']
        self.oper = struct_name_split[1]
        self.access_pat = struct_name_split[2]
        self.block_size = int(struct_name_split[4])
        self.cutoff, self.is_hybrid = -1, False
        if len(struct_name_split) > 7:
            self.cutoff = int(struct_name_split[7])
            self.is_hybrid = True
        self.time = float(data["cpu_time"])
        self.space = float(data["Space"])


plt.rcParams["figure.figsize"] = (10, 15)

file_path = "special.txt"

NUM_OF_QUERIES = 10000.0
ONES_PERCENTAGE = 5
results = []
markers = ["x", "s"]
colors = ['r', 'g', 'b', 'darkorange']

with open(file_path, "r") as f:
    data = json.load(f)
    results = [BenchRes(i) for i in data["benchmarks"]]

fig, axs = plt.subplots(3)
fig.suptitle(
    f"Access, rank, select na postupnosti dĺžky 1 << 25 prvkov ({ONES_PERCENTAGE}% jednotiek)")

isHybrid = [True, False]

for result_index in range(2):
    for (index1, access_pattern) in enumerate(["AccessPattern::Random"]):
        for (index2, oper) in enumerate(["Operation::Access", "Operation::Rank", "Operation::Select"]):
            graph_index = 3*index1+index2

            for (index3, block_size) in enumerate([15, 31, 63, 127]):
                x = [res.space for res in results if (res.oper, res.access_pat, res.block_size, res.is_hybrid) == (
                    oper, access_pattern, block_size, isHybrid[result_index])]
                y = [res.time/NUM_OF_QUERIES for res in results if (res.oper, res.access_pat, res.block_size, res.is_hybrid) == (
                    oper, access_pattern, block_size, isHybrid[result_index])]
                if x:
                    axs[graph_index].scatter(x, y, label=str(result_index) + "-" + str(
                        block_size), marker=markers[result_index], c=colors[index3])

            axs[graph_index].legend(loc='center left',
                                        bbox_to_anchor=(1, 0.5))
            axs[graph_index].set_title(translate(oper + " " + access_pattern))
            axs[graph_index].set_xlabel("bitov na bit")
            axs[graph_index].set_ylabel("čas (ns)")

            x = [res.space for res in results if (
                res.oper, res.access_pat, res.block_size) == (oper, access_pattern, 0)]
            y = [res.time/NUM_OF_QUERIES for res in results if (
                res.oper, res.access_pat, res.block_size) == (oper, access_pattern, 0)]
            axs[graph_index].scatter(x, y, label=str(result_index) +
                                     "-" + str(block_size), marker='*', c="0.0")

fig.tight_layout(pad=3.0)

plt.savefig("vysledky_nase.png", bbox_inches='tight')
plt.clf()

#
#axs[1].plot(x, -y)
