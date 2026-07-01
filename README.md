# C++ Math Expression Evaluator

A fast, command-line mathematical expression evaluator written in C++. It parses and calculates human-readable string expressions using Dijkstra's **Shunting-Yard algorithm**, converting standard infix notation (e.g., `3 + 4 * 2`) into postfix notation (Reverse Polish Notation) for efficient evaluation.

## ✨ Features

* **Interactive & CLI Modes:** Run it as a continuous interactive REPL (Read-Eval-Print Loop) or pass a single expression directly via command-line arguments.
* **Microsecond Benchmarking:** Every calculation automatically tracks and prints its execution time in microseconds (`us`).
* **Robust Error Handling:** Tracks character positions during tokenization and parsing to point out exactly where syntax errors, missing parenthesis, or invalid arguments occur.
* **Standard Math Library Integration:** Supports a wide array of trigonometric and logarithmic functions, as well as core mathematical constants.

## 🧮 Supported Operations

**Basic Operators:**

* Addition (`+`), Subtraction (`-`), Multiplication (`*`), Division (`/`)
* Unary plus and minus (e.g., `-5 + +3`)
* Parentheses for grouping `()`

**Constants:**

* `pi` ($\approx 3.14159$)
* `e` ($\approx 2.71828$)

**Functions:**

* **Trigonometry:** `sin(x)`, `cos(x)`, `tan(x)` (Calculated in radians)
* **Logarithms:** `log(x)` (Natural log), `log10(x)` (Base-10 log)
* **Exponents & Roots:** `exp(x)`, `sqrt(x)`, `pow(x, y)`

## 🛠 Technical Overview

The calculation pipeline is divided into three distinct phases:

1. **Tokenization (`tokenize`):** Scans the raw string and breaks it down into actionable `Token` objects (Numbers, Operators, Functions, Parentheses, Commas). It smartly distinguishes between binary operators (subtraction) and unary operators (negative numbers).
2. **Parsing (`parse`):** Implements the Shunting-Yard algorithm to reorder the tokens from standard Infix notation into Postfix notation, discarding parentheses and sorting by operator precedence.
3. **Evaluation (`evaluate`):** Iterates through the Postfix tokens using a stack to compute the final floating-point (`double`) result.

## 🚀 Compilation & Usage

### Compilation

This program uses modern C++ features (like `<chrono>` and `<stdexcept>`). Compile it using any standard C++ compiler with at least C++11 support:

```bash
g++ -std=c++11 main.cpp -o cppcalculator

```

### 1. Interactive Mode (REPL)

Run the executable without any arguments to enter the interactive shell.

```bash
./cppcalculator

```

**Example Session:**

```text
cppcalculator via Shunting-Yard (type 'exit' to quit).
> 5 + 3 * (10 - 2)
ans = 29 (completed in 12 us)
> pow(2, 3) * pi
ans = 25.1327 (completed in 15 us)
> sin(pi / 2)
ans = 1 (completed in 11 us)
> 5 + * 2
ERROR at: 4 (completed in 4 us)
> exit

```

### 2. Single Execution Mode

Pass your mathematical expression directly as command-line arguments. The program will evaluate it, print the result, and exit immediately.

```bash
./cppcalculator "sqrt(144) + 10"
# Output: ans = 22 (completed in 10 us)

./cppcalculator 10 / 2 + log(e)
# Output: ans = 6 (completed in 13 us)

```
