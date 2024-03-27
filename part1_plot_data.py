import matplotlib.pyplot as plt
import math

threads = [0, 1, 2, 4, 8, 16]
data_20 = []
data_100 = []
data_1000 = []

files = ['./Part1Data/results_no_omp.txt',
         './Part1Data/results_1_thread.txt',
         './Part1Data/results_2_thread.txt',
         './Part1Data/results_4_thread.txt',
         './Part1Data/results_8_thread.txt',
         './Part1Data/results_16_thread.txt']

for f in files:
    with open(f, 'r') as file:
        for line in file:
            parts = line.split()
            if len(parts) != 3:
                continue
            thread = int(parts[0])
            size = int(parts[1])
            time = float(parts[2])
            if size == 20:
                data_20.append(time)
            elif size == 100:
                data_100.append(time)
            elif size == 1000:
                data_1000.append(time)


plt.figure(figsize=(10, 6))
plt.yscale("log")
plt.xscale("log")
plt.title(f"Time vs # Threads")
plt.xlabel('Threads')
plt.ylabel('Time (seconds)')
plt.grid(True)
plt.plot(threads, data_20, marker='o', color='r', linestyle='-', label="N=20")
plt.plot(threads, data_100, marker='o', color='g', linestyle='-', label="N=100")
plt.plot(threads, data_1000, marker='o', color='b', linestyle='-', label="N=1000")
plt.legend()
plt.savefig("./part1_plot.png")

