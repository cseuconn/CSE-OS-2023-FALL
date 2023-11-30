import matplotlib.pyplot as plt

METHOD = "single"

num_runs = range(1, 10001)
atomic_cycles = []
cond_cycles = []
mutex_cycles = []
sem_cycles = []
spin_cycles = []

atomic_time = []
cond_time = []
mutex_time = []
sem_time = []
spin_time = []

with open(f"results/overhead/{METHOD}_init/atomic.txt") as f:
	for line in f:
		line = line.split('\t')
		cycles = line[0].split()[0]
		time = line[1].split()[0]
		atomic_cycles.append(int(cycles))
		atomic_time.append(float(time))

with open(f"results/overhead/{METHOD}_init/conditional.txt") as f:
	for line in f:
		line = line.split('\t')
		cycles = line[0].split()[0]
		time = line[1].split()[0]
		cond_cycles.append(int(cycles))
		cond_time.append(float(time))

with open(f"results/overhead/{METHOD}_init/mutex.txt") as f:
	for line in f:
		line = line.split('\t')
		cycles = line[0].split()[0]
		time = line[1].split()[0]
		mutex_cycles.append(int(cycles))
		mutex_time.append(float(time))

with open(f"results/overhead/{METHOD}_init/semaphore.txt") as f:
	for line in f:
		line = line.split('\t')
		cycles = line[0].split()[0]
		time = line[1].split()[0]
		sem_cycles.append(int(cycles))
		sem_time.append(float(time))

with open(f"results/overhead/{METHOD}_init/spin.txt") as f:
	for line in f:
		line = line.split('\t')
		cycles = line[0].split()[0]
		time = line[1].split()[0]
		spin_cycles.append(int(cycles))
		spin_time.append(float(time))

def plot_cycles() -> None:
	fig, ax = plt.subplots()

	ax.set_title("Number of cycles for each number of iterations")
	ax.set_xlabel("Number of iterations")
	ax.set_ylabel("Number of cycles")
	ax.grid(True, 'major', 'y')

	ax.plot(num_runs, atomic_cycles)
	ax.plot(num_runs, cond_cycles)
	ax.plot(num_runs, mutex_cycles)
	ax.plot(num_runs, sem_cycles)
	ax.plot(num_runs, spin_cycles)

	ax.legend(["Atomic", "Conditional", "Mutex", "Semaphore", "Spinlock"])

def plot_times() -> None:
	fig, ax = plt.subplots()

	ax.set_title("Time for each number of iterations")
	ax.set_xlabel("Number of iterations")
	ax.set_ylabel("Time (seconds)")
	ax.grid(True, 'major', 'y')

	ax.plot(num_runs, atomic_time)
	ax.plot(num_runs, cond_time)
	ax.plot(num_runs, mutex_time)
	ax.plot(num_runs, sem_time)
	ax.plot(num_runs, spin_time)

	ax.legend(["Atomic", "Conditional", "Mutex", "Semaphore", "Spinlock"])

if __name__ == "__main__":
	plot_cycles()
	#plot_times()

	plt.show()