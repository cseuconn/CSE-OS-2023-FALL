import matplotlib.pyplot as plt

times = {
	1	: 3.125979,
	2	: 1.605169,
	3	: 0.981415,
	4	: 0.773798,
	5	: 0.615747,
	6	: 0.529481,
	7	: 0.601779,
	8	: 0.536474,
	9	: 0.540353,
	10	: 0.565209,
	11	: 0.607652,
	12	: 0.549279,
	13	: 0.549881,
	14	: 0.548685,
	15	: 0.546386
}

def plot_times() -> None:
	fig, ax = plt.subplots()

	ax.set_title("Time for each number of iterations")
	ax.set_xlabel("Number of iterations")
	ax.set_ylabel("Time (seconds)")
	ax.grid(True, 'major', 'y')

	ax.bar(times.keys(), times.values())

if __name__ == "__main__":
	plot_times()

	plt.show()