import json
import re
import matplotlib.pyplot as plt

plt.rcParams["figure.figsize"] = (10,12)

file_paths = ["old_output.txt", "new_output.txt"]

results = []
markers = ["s", "o"]

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

fig, axs = plt.subplots(4)
fig.suptitle('Vertically stacked subplots')

for result_index in range(len(results)):
    for (index1, operation) in enumerate(["Operation::Access", "Operation::Rank"]):
        for (index2, access_pattern) in enumerate(["AccessPattern::Random", "AccessPattern::ContinuousRandom"]):
            for block_size in [15, 31, 63, 127]:
                x = [res[1] for (par, res) in results[result_index] if par[0] == operation and par[1] == access_pattern and int(par[3]) == block_size]
                y = [res[0] for (par, res) in results[result_index] if par[0] == operation and par[1] == access_pattern and int(par[3]) == block_size]
                axs[2*index1+index2].scatter(x, y, label = str(result_index) + "-" + str(block_size), marker = markers[result_index])
            axs[2*index1+index2].legend(loc='center left', bbox_to_anchor=(1, 0.5))
            axs[2*index1+index2].set_title(operation + " " + access_pattern)

fig.tight_layout(pad=3.0)

plt.savefig("vysledky_nase.png", bbox_inches='tight')
plt.clf()

#
#axs[1].plot(x, -y)