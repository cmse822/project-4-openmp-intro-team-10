import matplotlib.pyplot as plt
import math

threads = []
data = []


files = ['./HPCCData/diffThreads1.out']
        #  './HPCCData/diffRanks-5threads.out',
        #  './HPCCData/diffRanks-10threads.out',]

for f in files:
    with open(f, 'r') as file:
        for line in file:
            parts = line.split()
            if len(parts) != 4:
                continue
            rank = int(parts[0])
            thread = int(parts[1])
            size = int(parts[2])
            time = float(parts[3])
            threads.append(thread)
            data.append(time)

print(threads)
print(data)
plt.figure(figsize=(10, 6))
plt.yscale("log")
plt.xscale("log")
plt.title(f"Hybrid MMM 1000x1000 Matrix, 10 Ranks - Time vs Threads")
plt.xlabel('Threads')
plt.ylabel('Time (seconds)')
plt.grid(True)
plt.plot(threads, data, marker='o', color='g', linestyle='-')
# plt.legend()
plt.savefig("./part3_different_threads0_plot.png")

# CODE FOR PLOTTING DIFFERING Ranks

# threads = [0, 1, 2, 4, 8, 16]
# ranks = []
# data_1 = []
# data_5 = []
# data_10 = []

# files = ['./HPCCData/diffRanks-1thread.out',
#          './HPCCData/diffRanks-5threads.out',
#          './HPCCData/diffRanks-10threads.out',]
# for f in files:
#     with open(f, 'r') as file:
#         for line in file:
#             parts = line.split()
#             if len(parts) != 4:
#                 continue
#             rank = int(parts[0])
#             thread = int(parts[1])
#             size = int(parts[2])
#             time = float(parts[3])
#             if thread == 1:
#                 ranks.append(rank)
#                 data_1.append(time)
#             elif thread == 5:
#                 data_5.append(time)
#             elif thread == 10:
#                 data_10.append(time)

# plt.figure(figsize=(10, 6))
# plt.yscale("log")
# plt.xscale("log")
# plt.title(f"Hybrid MMM 1000x1000 Matrix - Time vs Rank")
# plt.xlabel('Rank')
# plt.ylabel('Time (seconds)')
# plt.grid(True)
# plt.plot(ranks, data_1, marker='o', color='r', linestyle='-', label="1 Thread")
# plt.plot(ranks, data_5, marker='o', color='g', linestyle='-', label="5 Threads")
# plt.plot(ranks, data_10, marker='o', color='b', linestyle='-', label="10 Threads")
# plt.legend()
# plt.savefig("./part3_different_ranks_plot.png")

