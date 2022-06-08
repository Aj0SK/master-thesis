import json
import re
import matplotlib.pyplot as plt

def translate(str):
    preklady = [["Operation", "Operácia"], ["AccessPattern", "PrístupovýTyp"],
    ["Access", "Prístup"], ["ContinuousRandom", "NáhodnýLineárny"], ["Random", "Náhodný"]]
    for preklad in preklady:
        str = str.replace(preklad[0], preklad[1])
    return str

plt.rcParams["figure.figsize"] = (10,6)

file_paths = ["old_output.txt", "new_output.txt"]

results = []
markers = ["o", "x"]
names = ["sdsl", "my"]
colors = ['r', 'g', 'b', 'darkorange']
NUM_OF_QUERIES = 10000.0
OP = "Rank" # Rank

for file_path in file_paths:
    tests = []
    with open(file_path, "r") as f:
        data = json.load(f)
        for i in data["benchmarks"]:
            name = i["name"]
            time = i["cpu_time"]
            space = i["Space"]
            it = i["iterations"]
            splited_name = re.split(r"[,<]", name)
            splited_name = [i.strip() for i in splited_name if i != '']
            tests.append((splited_name[1:], [time, space]))
    results.append(tests)

print(results)

fig, axs = plt.subplots(1)
fig.suptitle(f'{OP} na postupnosti dĺžky 100 000 prvkov (10% jednotiek - entropia 0.47)', fontsize=20)

for result_index in range(len(results)):
    for (index1, access_pattern) in enumerate(["AccessPattern::Random"]):
        for (index2, operation) in enumerate([f"Operation::{OP}"]):
            for (index3, block_size) in enumerate([15, 31, 63, 127]):
                x = [res[1] for (par, res) in results[result_index] if par[0] == operation and par[1] == access_pattern and int(par[3]) == block_size]
                y = [res[0]/NUM_OF_QUERIES for (par, res) in results[result_index] if par[0] == operation and par[1] == access_pattern and int(par[3]) == block_size]
                if block_size == 15 and names[result_index] == "my":
                    pass
                else:
                    axs.scatter(x, y, label = names[result_index] + "-" + str(block_size), marker = markers[result_index], c = colors[index3], s = 120)
            axs.legend(loc='center left', bbox_to_anchor=(1, 0.5))
            axs.set_xlabel("bitov/bit", fontsize=16)
            axs.set_ylabel("čas (ns)", fontsize=16)

fig.tight_layout(pad=3.0)

plt.savefig(f"vysledky_nase_{OP}.png", bbox_inches='tight')
plt.clf()