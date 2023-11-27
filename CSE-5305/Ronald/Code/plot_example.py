import matplotlib.pyplot as plt

cycles = {
	"atomic" 	: 214,
	"spinlock"	: 72,
	"mutex"		: 86,
	"semaphore"	: 173
}
times = {
	"atomic" 	: 0.000112,
	"spinlock"	: 0.000044,
	"mutex"		: 0.000056,
	"semaphore"	: 0.000146
}

def plot_cycles() -> None:
	fig, ax = plt.subplots()

	ax.set_title("Number of cycles for each number of iterations")
	ax.set_xlabel("Number of iterations")
	ax.set_ylabel("Number of cycles")
	ax.grid(True, 'major', 'y')

	ax.bar(cycles.keys(), cycles.values())

def plot_times() -> None:
	fig, ax = plt.subplots()

	ax.set_title("Time for each number of iterations")
	ax.set_xlabel("Number of iterations")
	ax.set_ylabel("Time (seconds)")
	ax.grid(True, 'major', 'y')

	ax.bar(times.keys(), times.values())

if __name__ == "__main__":
	plot_cycles()
	plot_times()

	plt.show()