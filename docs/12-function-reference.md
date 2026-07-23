# 12. Function Reference

A detailed explanation of every type and function, grouped by module. File paths
link to the source.

## 12.1 `PriceArm` ([include/PriceArm.hpp](../include/PriceArm.hpp))

A plain struct representing one arm of the bandit = one candidate price.

| Member | Meaning |
|--------|---------|
| `price` | The concrete selling price this arm offers. |
| `pullCount` (`n`) | Number of times the arm has been selected. |
| `totalReward` | Running sum of observed rewards (for reporting). |
| `estimatedReward` (`Q`) | Incrementally maintained mean reward. |

- `PriceArm()` / `PriceArm(double p)` - construct with all statistics zeroed;
  the second form sets the price.

## 12.2 `SimulationConfig` ([include/SimulationConfig.hpp](../include/SimulationConfig.hpp))

Groups every parameter of a run so no globals are needed.

| Member | Meaning |
|--------|---------|
| `basePrice` | Reference price; candidate prices are offsets of it. |
| `numRounds` (`n`) | Number of pricing rounds to simulate. |
| `demandLevel` (`d`) | Overall willingness to buy, in `(0, 1]`. |
| `competitorPrice` | Competitor's price; centres the demand curve. |
| `inventoryLevel` | Total units available across the whole run. |
| `epsilon` | Exploration probability, in `[0, 1]`. |
| `customersPerRound` (`m`) | Customers exposed to the price each round. |
| `costFraction` | Unit cost as a fraction of the base price. |
| `seed` | PRNG seed for reproducibility. |

- `unitCost()` - returns `basePrice * costFraction`, the absolute unit cost used
  to compute margins.

## 12.3 `EpsilonGreedyBandit` ([include/EpsilonGreedyBandit.hpp](../include/EpsilonGreedyBandit.hpp), [src](../src/EpsilonGreedyBandit.cpp))

The domain-agnostic decision policy. Owns its arms and its random engine.

- **`EpsilonGreedyBandit(arms, epsilon, seed)`** - constructor. Takes ownership of
  the arm vector, stores `epsilon`, and seeds a `std::mt19937`. Throws
  `std::invalid_argument` if there are no arms or `epsilon` is outside `[0, 1]`.

- **`std::size_t selectArm(bool* wasExploration = nullptr)`** - the action
  selection step. Draws `u ~ U[0,1)`; if `u < epsilon` it explores (returns a
  uniformly random arm index), otherwise it exploits (returns `bestArmIndex()`).
  If `wasExploration` is non-null it is set to indicate which branch was taken.
  Cost: `O(1)` for explore, `O(k)` for exploit.

- **`void updateArm(std::size_t armIndex, double reward)`** - the reward-update
  step. Increments the arm's `pullCount` and `totalReward`, then applies the
  incremental-average update
  `Q += (reward - Q) / pullCount`. Throws `std::out_of_range` on a bad index.
  Cost: `O(1)`.

- **`std::size_t bestArmIndex() const`** - linear scan returning the index of the
  arm with the greatest `estimatedReward`; ties go to the lowest index. Cost:
  `O(k)`.

- **`const std::vector<PriceArm>& arms() const`** / **`std::size_t armCount() const`**
  - read-only accessors for the arms and their count.

## 12.4 `MarketSimulator` ([include/MarketSimulator.hpp](../include/MarketSimulator.hpp), [src](../src/MarketSimulator.cpp))

The stochastic environment: turns a price into units sold and profit. Owns a
separate PRNG so market noise is independent of the bandit's exploration draws.

- **`MarketSimulator(const SimulationConfig& config)`** - copies the config,
  initialises `remainingInventory` to `inventoryLevel`, and seeds its engine with
  an offset of `config.seed` (decorrelated but reproducible).

- **`double purchaseProbability(double price) const`** - evaluates the logistic
  demand curve centred on the competitor price, scaled by `demandLevel`, clamped
  to `[0, 1]`. This is the per-customer probability of buying.

- **`double expectedProfit(double price) const`** - the noise-free expected
  profit per round, `customersPerRound * purchaseProbability(price) *
  (price - unitCost)`. Used only to identify the optimal arm and to compute
  regret; the bandit never sees it.

- **`RoundOutcome simulateRound(double price)`** - simulates one round: up to
  `customersPerRound` independent Bernoulli purchase decisions, capped by
  remaining inventory, decrementing inventory as units sell. Returns the units
  sold and the profit `(price - unitCost) * unitsSold`. Cost: `O(m)`.

- **`std::size_t remainingInventory() const`** - units still available.

- **`void reset()`** - restores inventory to its initial level so the simulator
  can be reused for another run.

- **`struct RoundOutcome { unitsSold; profit; }`** - the result of one round.

## 12.5 `PricingEngine` ([include/PricingEngine.hpp](../include/PricingEngine.hpp), [src](../src/PricingEngine.cpp))

Orchestrates the whole simulation and renders reports. Holds the config, the
generated candidate prices, and a `MarketSimulator`.

- **`PricingEngine(const SimulationConfig& config)`** - stores the config, builds
  the market, and calls `generateCandidatePrices()`.

- **`void generateCandidatePrices()`** (private) - fills `candidatePrices_` with
  `basePrice * (1 + offset)` for each of the fixed offsets
  `{-0.10, -0.05, 0, +0.05, +0.10}`. This fixes `k = 5`.

- **`Report run(std::ostream& out, bool verbose)`** - the main learning loop:
  builds one arm per price, constructs the bandit, determines the optimal arm via
  `expectedProfit` (for regret), then for each of `n` rounds performs
  select -> simulate -> update, accumulating total profit and regret and printing
  progress (every round if `verbose`, else ~10 snapshots). Returns a filled
  `Report`. Cost: `O(n*(k + m))`.

- **`void printRoundLine(...)`** (private) - formats a single verbose round line
  (explore/exploit, price, units sold, reward, updated `Q`).

- **`void printFinalReport(std::ostream& out, const Report& report) const`** -
  prints the per-price table (times used, average reward, total reward), marks the
  chosen and optimal prices, and prints total profit and cumulative regret.

- **`const std::vector<double>& candidatePrices() const`** - the generated prices.

- **`struct Report`** - aggregate results: final `arms`, `totalProfit`,
  `cumulativeRegret`, `bestArmIndex` (max `Q`), and `optimalArmIndex` (max
  expected profit).

## 12.6 `main` ([src/main.cpp](../src/main.cpp))

The console front end.

- **`template <typename T> T readValue(prompt, minValue, maxValue, defaultValue)`**
  - prompts, reads a whole line, accepts the default on an empty line, parses the
  value, and re-prompts until it is numeric and within `[minValue, maxValue]`.
  This makes the program robust to malformed input.

- **`int main()`** - prints a banner, collects and validates every parameter via
  `readValue`, constructs a `PricingEngine`, prints the candidate prices, runs the
  simulation, and prints the final report.
