import threading

class SudokuSolver:
    def __init__(self, puzzle_size, initial_state):
        self.size = puzzle_size
        self.board = [[0] * self.size for _ in range(self.size)]
        self.mutex = threading.Lock()
        self.cv = threading.Condition(self.mutex)
        self.finished_threads = 0

        # Copy the initial state to the board
        for i in range(self.size):
            for j in range(self.size):
                self.board[i][j] = initial_state[i][j]

    def is_valid(self, row, col, num):
        for i in range(self.size):
            if (
                self.board[row][i] == num
                or self.board[i][col] == num
                or self.board[3 * (row // 3) + i // 3][3 * (col // 3) + i % 3] == num
            ):
                return False
        return True

    def find_empty_cell(self):
        for i in range(self.size):
            for j in range(self.size):
                if self.board[i][j] == 0:
                    return (i, j)
        return None

    def solve_sudoku(self):
        empty_cell = self.find_empty_cell()
        if not empty_cell:
            return True

        row, col = empty_cell

        for num in range(1, 10):
            if self.is_valid(row, col, num):
                with self.mutex:
                    self.board[row][col] = num

                if self.solve_sudoku():
                    return True

                with self.mutex:
                    self.board[row][col] = 0

        with self.mutex:
            self.finished_threads += 1
            self.cv.notify()

        return False

    def solve(self):
        threads = []
        for _ in range(self.size):
            thread = threading.Thread(target=self.solve_sudoku)
            threads.append(thread)
            thread.start()

        with self.mutex:
            while self.finished_threads < self.size:
                self.cv.wait()

    def print_board(self):
        for row in self.board:
            print(row)

def main():
    puzzle_size = 9
    
    #example board
    initial_state = [
        [0, 0, 0, 0, 0, 0, 1, 6, 0],
        [0, 0, 0, 9, 0, 0, 0, 0, 5],
        [4, 3, 0, 0, 2, 0, 0, 0, 0],
        [0, 0, 1, 0, 0, 0, 0, 4, 0],
        [6, 0, 0, 0, 5, 0, 0, 3, 0],
        [0, 0, 8, 1, 0, 0, 0, 0, 7],
        [0, 7, 0, 0, 0, 0, 0, 0, 2],
        [5, 9, 0, 2, 7, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 5, 0],
    ]

    sudoku_solver = SudokuSolver(puzzle_size, initial_state)

    sudoku_solver.solve()
    sudoku_solver.print_board()

if __name__ == "__main__":
    main()


    