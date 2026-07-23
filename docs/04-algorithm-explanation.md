# 4. Algorithm Explanation

The engine uses the **epsilon-Greedy Multi-Armed Bandit** algorithm. This
document explains each of its five moving parts and maps them to the code.

## 4.1 The five building blocks

| Concept | What it does | Where in the code |
|---------|--------------|-------------------|
| Action selection | Decides which price to try this round | `EpsilonGreedyBandit::selectArm` |
| Exploration | Occasionally picks a random price to learn | `selectArm` (random branch) |
| Exploitation | Usually picks the current best price | `selectArm` -> `bestArmIndex` |
| Reward updating | Folds the observed profit into an estimate | `EpsilonGreedyBandit::updateArm` |
| Incremental average | Maintains the running mean in O(1) memory | `updateArm` (the update formula) |

## 4.2 State maintained per arm

For each candidate price (arm) `i` the algorithm stores just three numbers
(see [`PriceArm.hpp`](../include/PriceArm.hpp)):

- `n_i` - the number of times the arm has been pulled (`pullCount`).
- `Q_i` - the current estimate of the arm's mean reward (`estimatedReward`).
- `totalReward` - the running sum of rewards (kept for reporting).

Only `n_i` and `Q_i` are needed by the algorithm itself; both are `O(1)` per arm.

## 4.3 Action selection: epsilon-Greedy

Each round the policy draws a uniform random number `u in [0, 1)`:

```
if  u < epsilon:   EXPLORE  -> choose an arm uniformly at random
else:              EXPLOIT  -> choose argmax_i Q_i
```

- With probability **epsilon** the engine *explores*: it ignores its current
  beliefs and tries a random price. This guarantees every price keeps being
  sampled, so no arm's estimate can stay wrong forever.
- With probability **1 - epsilon** the engine *exploits*: it charges the price
  with the highest current estimated reward, which is where the profit is.

`epsilon` is the single knob that trades learning against earning. `epsilon = 0`
is pure greedy (never explores; can get stuck); `epsilon = 1` is pure random
(always explores; never cashes in). Typical values are small, e.g. `0.05`-`0.15`.

### Code

```cpp
std::size_t EpsilonGreedyBandit::selectArm(bool* wasExploration) {
    const bool explore = uniform01_(rng_) < epsilon_;      // draw u, compare
    if (wasExploration) *wasExploration = explore;
    if (explore) {                                         // EXPLORE
        std::uniform_int_distribution<std::size_t> pick(0, arms_.size() - 1);
        return pick(rng_);
    }
    return bestArmIndex();                                 // EXPLOIT
}
```

Exploitation calls `bestArmIndex`, a linear scan over the `k` arms returning the
index of the largest `Q_i` (ties broken toward the lowest index for
determinism).

## 4.4 Reward updating and the incremental average

After a price is offered, the market returns a reward `R` (the round's profit).
The naive way to update the arm's mean would be to store every reward and
re-average, costing `O(n_i)` time and memory. Instead we use the **incremental
(online) average**, which gives the *exact* same mean using `O(1)` time and
memory:

```
n_i     <- n_i + 1
Q_i     <- Q_i + (1 / n_i) * (R - Q_i)
```

The term `(R - Q_i)` is the *prediction error*: how surprising the new reward was
compared to what we expected. We nudge the estimate toward the new observation by
a fraction `1/n_i` of that error. Early on (small `n_i`) the step is large and
the estimate moves quickly; later (large `n_i`) the steps shrink and the estimate
settles. A short derivation is in the [Mathematical Model](05-mathematical-model.md).

### Code

```cpp
void EpsilonGreedyBandit::updateArm(std::size_t armIndex, double reward) {
    PriceArm& arm = arms_[armIndex];
    arm.pullCount   += 1;
    arm.totalReward += reward;
    arm.estimatedReward += (reward - arm.estimatedReward)
                           / static_cast<double>(arm.pullCount);
}
```

## 4.5 Putting it together: one round

```
1. a  <- selectArm()                 # explore or exploit
2. R  <- market.simulateRound(p_a)   # observe noisy profit
3. updateArm(a, R)                    # incremental-average update
4. accumulate total profit and regret; report
```

Repeating this loop for `n` rounds is the entire algorithm. Because exploration
keeps sampling every price while the estimates converge to the true means, the
greedy choice increasingly coincides with the optimal price. The formal argument
is in [Correctness & Convergence](09-correctness-and-convergence.md).
