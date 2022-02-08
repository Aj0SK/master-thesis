import json
import re
import matplotlib.pyplot as plt

def translate(str):
    preklady = [["Operation", "Operácia"], ["AccessPattern", "PrístupovýTyp"],
    ["Access", "Prístup"], ["ContinuousRandom", "NáhodnýLineárny"], ["Random", "Náhodný"]]
    for preklad in preklady:
        str = str.replace(preklad[0], preklad[1])
    return str

plt.rcParams["figure.figsize"] = (10,15)

file_paths = ["special.txt"]

results = []
markers = ["s", "x"]

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

fig, axs = plt.subplots(6)
fig.suptitle('Prístup, rank, select na postupnosti dĺžky 100 000 prvkov (5% jednotiek - 0.29)')

isHybrid = [True, False]

for result_index in range(2):
    for (index1, access_pattern) in enumerate(["AccessPattern::Random", "AccessPattern::ContinuousRandom"]):
        for (index2, operation) in enumerate(["Operation::Access", "Operation::Rank", "Operation::Select"]):
            for block_size in [15, 31, 63, 127]:
                x = [res[1] for (par, res) in results[0] if par[0] == operation and par[1] == access_pattern and int(par[3]) == block_size and (isHybrid[result_index] == sum([1 for i in range(256) if (str(i) + ">") in par]))]
                y = [res[0] for (par, res) in results[0] if par[0] == operation and par[1] == access_pattern and int(par[3]) == block_size and (isHybrid[result_index] == sum([1 for i in range(256) if (str(i) + ">") in par]))]
                if x:
                    axs[3*index1+index2].scatter(x, y, label = str(result_index) + "-" + str(block_size), marker = markers[result_index])
            axs[3*index1+index2].legend(loc='center left', bbox_to_anchor=(1, 0.5))
            axs[3*index1+index2].set_title(translate(operation + " " + access_pattern))

fig.tight_layout(pad=3.0)

plt.savefig("vysledky_nase.png", bbox_inches='tight')
plt.clf()

#
#axs[1].plot(x, -y)