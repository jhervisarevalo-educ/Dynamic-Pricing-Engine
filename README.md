# Dynamic Pricing Engine (epsilon-Greedy Multi-Armed Bandit)

A console C++17 application that learns the most profitable selling price for a
product by experimenting with candidate prices and observing simulated profit,
using the **epsilon-Greedy Multi-Armed Bandit** algorithm implemented from
scratch (no machine-learning or external libraries).

## Requirements

- A C++17 compiler (GCC, Clang, or MSVC) and [CMake](https://cmake.org) >= 3.14.
- No external libraries; only the C++ standard library is used, so the project
  builds and runs on **Linux, macOS, and Windows**.

## Quick start

Configure and build (works on any platform):

```bash
cmake -S . -B build
cmake --build build --config Release
```

Then run the executable. Its location depends on the CMake generator:

- **Linux / macOS** (single-config: Make, Ninja):

  ```bash
  ./build/pricing_engine
  ```

- **Windows** (multi-config: Visual Studio) using PowerShell or `cmd`:

  ```bat
  .\build\Release\pricing_engine.exe
  ```

  (If you build with a single-config generator such as Ninja or MinGW
  Makefiles, the binary is at `.\build\pricing_engine.exe` instead.)

The program is interactive; press Enter at any prompt to accept the shown
default.

### Running non-interactively

Answers are read in prompt order: base price, rounds, demand, competitor price,
inventory, epsilon, customers/round, cost fraction, verbose flag.

- **Linux / macOS / Git Bash / WSL** (any POSIX shell):

  ```bash
  printf "100\n2000\n0.6\n100\n100000\n0.1\n100\n0.6\n0\n" | ./build/pricing_engine
  ```

- **Windows PowerShell** (`printf` is unavailable; feed lines instead):

  ```powershell
  "100","2000","0.6","100","100000","0.1","100","0.6","0" | .\build\Release\pricing_engine.exe
  ```

- **Any OS**: put one answer per line in a file (e.g. `input.txt`) and redirect:

  ```bash
  ./build/pricing_engine < input.txt
  ```

## What it does

1. Generates candidate prices around the base price (-10%, -5%, 0%, +5%, +10%).
2. Each round, uses epsilon-Greedy to **explore** a random price or **exploit**
   the current best, simulates customer purchases, and computes the profit.
3. Updates each price's estimated reward with an incremental average.
4. Reports per-price usage counts, average reward, total profit, the
   best-performing price, and cumulative regret.

## Documentation

Full documentation lives in [`docs/`](docs/00-index.md), covering the problem,
algorithm, mathematical model, pseudocode, flowcharts, correctness and
convergence, time/space complexity, sample runs, and a per-function reference.

## Source layout

```
include/   Public headers (PriceArm, SimulationConfig, EpsilonGreedyBandit,
           MarketSimulator, PricingEngine)
src/       Implementations + main.cpp
docs/      Course documentation (start at docs/00-index.md)
```
