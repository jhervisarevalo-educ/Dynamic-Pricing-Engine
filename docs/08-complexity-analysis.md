# 8. Time and Space Complexity Analysis

Throughout, let:

- `n` = number of pricing rounds (`numRounds`),
- `k` = number of candidate prices / arms (fixed at 5 by default),
- `m` = customers simulated per round (`customersPerRound`).

## 8.1 Cost of one round

A single round performs:

1. **Action selection** (`selectArm`):
   - Exploration: draw one random number, one random index -> `O(1)`.
   - Exploitation: `bestArmIndex` scans all arms -> `O(k)`.
   - Worst case per round: `O(k)`.
2. **Market simulation** (`simulateRound`): up to `m` Bernoulli draws -> `O(m)`.
3. **Reward update** (`updateArm`): a constant number of arithmetic operations
   thanks to the incremental average -> `O(1)`.
4. **Bookkeeping / printing**: `O(1)` (verbose printing is a constant amount of
   text per round).

So one round costs `O(k + m)`.

## 8.2 Total time complexity

Over `n` rounds:

```
T(n) = n * O(k + m) = O(n*k + n*m)
```

There is also `O(k)` setup work (generating prices, initialising arms) and a
final report costing `O(k)`. These are dominated by the loop.

### Focusing on the algorithm itself

The Multi-Armed Bandit *algorithm* (selection + update, excluding the external
market model) costs `O(k)` per round and therefore:

```
T_algorithm(n) = O(n * k)
```

The `n*m` term comes from the *environment* (simulating individual customers),
not from the bandit. If the environment returned a reward in `O(1)` (e.g. a
direct formula), the total would be exactly `O(n*k)`.

## 8.3 Best / average / worst case

The algorithm is **input-oblivious** in its control flow: it always runs `n`
rounds and always does the same per-round work regardless of the reward values.
The only branch is explore vs. exploit.

| Case | Scenario | Per-round cost | Total |
|------|----------|----------------|-------|
| **Best** | Every step explores (`O(1)` selection) and `m` is treated as constant | `O(1)` selection, `O(m)` sim | `Theta(n*(1 + m))` |
| **Average** | Mix of explore/exploit; exploitation scan is `O(k)` | `O(k + m)` | `Theta(n*(k + m))` |
| **Worst** | Every step exploits (full `O(k)` scan each round) | `O(k + m)` | `Theta(n*(k + m))` |

Because `k` is a small fixed constant (see 8.6) and treating `m` as constant,
all three cases collapse to `Theta(n)`: the runtime is **linear in the number of
rounds**. This tight `Theta(n)` bound reflects that the work is essentially the
same no matter the data.

## 8.4 The roles of O(n), O(k), and O(nk)

- **O(n)** - the horizon term. Each additional round adds a fixed amount of work
  (selection + update), so runtime scales linearly with the number of rounds.
- **O(k)** - the per-round decision term. Choosing the greedy arm and (rarely)
  an exploration arm scans/relates to the `k` arms. Setup and the final report
  are also `O(k)`.
- **O(nk)** - the combined bound for the bandit algorithm: `n` rounds each doing
  `O(k)` work. This is the headline complexity of the method.

## 8.5 Space complexity

The algorithm stores, per arm, a constant number of scalars
(`price`, `pullCount`, `estimatedReward`, `totalReward`). With `k` arms:

```
S = O(k)
```

Critically, the **incremental average** means we do **not** store the history of
rewards. A naive estimator that kept every observed reward to re-average would
need `O(n*k)` (or `O(n)` total) memory; our approach is `O(k)` regardless of how
many rounds are run. The final report reuses the same `O(k)` structures.

## 8.6 Why `k` is treated as a constant

The number of candidate prices is **fixed by design** (five: -10%, -5%, base,
+5%, +10%). It does not grow with the input size `n`. When a quantity is a fixed
constant independent of the input, it is absorbed into the Big-O constant factor:

```
O(n * k)  with  k = 5   =>   O(5n)   =>   O(n)
```

Hence, for a fixed price grid, the engine's time complexity is effectively
**linear in the number of rounds**, `O(n)`, and its space complexity is
**constant**, `O(1)` (i.e. `O(k)` with `k` constant). We keep `k` explicit in the
analysis because the design *could* expose more price points; if `k` were allowed
to scale with the input, the `O(n*k)` form would be the honest bound.
