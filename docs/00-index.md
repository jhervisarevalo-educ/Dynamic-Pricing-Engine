# Dynamic Pricing Engine using an epsilon-Greedy Multi-Armed Bandit

Course project documentation for **Algorithms and Complexities**.

This documentation set explains the design, mathematics, correctness, and
complexity of a console C++ program that learns the most profitable selling
price for a product using the epsilon-Greedy Multi-Armed Bandit algorithm.

## Reading order

| # | Document | Contents |
|---|----------|----------|
| 01 | [Introduction](01-introduction.md) | What the project is and why it matters |
| 02 | [Problem Statement](02-problem-statement.md) | The dynamic pricing problem, formally |
| 03 | [Objectives](03-objectives.md) | Goals and success criteria |
| 04 | [Algorithm Explanation](04-algorithm-explanation.md) | Exploration, exploitation, reward update, action selection, incremental average |
| 05 | [Mathematical Model](05-mathematical-model.md) | Rewards, demand curve, incremental mean, regret |
| 06 | [Pseudocode](06-pseudocode.md) | Language-agnostic algorithm listing |
| 07 | [Flowchart](07-flowchart.md) | Mermaid and ASCII control-flow diagrams |
| 08 | [Time & Space Complexity](08-complexity-analysis.md) | Best/average/worst case, O(n), O(k), O(nk) |
| 09 | [Correctness & Convergence](09-correctness-and-convergence.md) | Why it converges to the optimal price |
| 10 | [Advantages, Limitations, Future Work](10-advantages-limitations-future.md) | Trade-offs and extensions |
| 11 | [Sample Runs](11-sample-runs.md) | Realistic executions and learning behaviour |
| 12 | [Function Reference](12-function-reference.md) | Detailed explanation of every function |

## Building and running

```bash
cmake -S . -B build
cmake --build build
./build/pricing_engine
```

The program is interactive; press Enter at any prompt to accept the shown
default. See [Sample Runs](11-sample-runs.md) for scripted (piped) executions.

## Source layout

```
Dynamic-Price-Engine/
├── CMakeLists.txt            # C++17 build (no external dependencies)
├── include/
│   ├── PriceArm.hpp          # One bandit arm = one candidate price
│   ├── SimulationConfig.hpp  # All run parameters
│   ├── EpsilonGreedyBandit.hpp
│   ├── MarketSimulator.hpp
│   └── PricingEngine.hpp
├── src/
│   ├── EpsilonGreedyBandit.cpp  # The algorithm core
│   ├── MarketSimulator.cpp      # Demand / profit environment
│   ├── PricingEngine.cpp        # Orchestration + reporting
│   └── main.cpp                 # Console I/O
└── docs/                        # This documentation set
```
