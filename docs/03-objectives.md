# 3. Objectives

## 3.1 Primary objective

Design and implement, from scratch, an epsilon-Greedy Multi-Armed Bandit
algorithm that recommends the most profitable selling price for a product by
learning from simulated sales, and demonstrate that it converges toward the
optimal price over time.

## 3.2 Functional objectives

The program must:

1. Run in the console and accept user input for:
   - Base price
   - Number of pricing rounds (simulated customers)
   - Demand level
   - Competitor price
   - Inventory level
2. Generate several candidate selling prices (default: -10%, -5%, base, +5%,
   +10%).
3. Use the epsilon-Greedy policy to decide which price to test each round.
4. Simulate customer purchases and compute the resulting profit.
5. Update the estimated reward of each price after every round using an
   incremental average.
6. Display the chosen price, the reward, and the updated statistics each round.
7. At the end, report:
   - Number of times each price was selected
   - Average reward for each price
   - Total profit
   - Best-performing price
   - Cumulative regret

## 3.3 Algorithmic objectives

- Implement exploration, exploitation, action selection, reward updating, and
  the incremental-average calculation **explicitly and from scratch**.
- Use **no** machine-learning or external AI libraries; only the C++ standard
  library, in particular `<random>`.
- Explain *why* the algorithm converges toward the optimal price.

## 3.4 Engineering objectives

- Object-oriented, modular, well-commented, production-quality C++17.
- Clear separation of concerns: the decision policy (bandit), the environment
  (market), and the orchestration (engine) are independent components.
- No global mutable state; randomness is owned by the objects that use it,
  making runs reproducible.

## 3.5 Success criteria

- The arm selected most often, and reported as best, matches the theoretically
  optimal arm (the one with the highest expected profit) on realistic inputs.
- Cumulative regret grows **sublinearly relative to total profit**: the fraction
  of profit lost to exploration shrinks as the horizon grows.
- Time complexity is `O(n*k)` and space complexity is `O(k)`, matching the
  analysis in [Complexity Analysis](08-complexity-analysis.md).
