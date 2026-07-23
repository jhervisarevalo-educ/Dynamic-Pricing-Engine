# 11. Sample Runs

All runs below are real, reproducible executions of `./build/pricing_engine`
(seed `42`). Inputs are supplied in prompt order; press Enter to accept defaults
when running interactively. Each run can be reproduced by piping the inputs, e.g.:

```bash
printf "100\n2000\n0.6\n100\n100000\n0.1\n100\n0.6\n0\n" | ./build/pricing_engine
```

The prompt order is: base price, rounds, demand level, competitor price,
inventory, epsilon, customers/round, cost fraction, verbose flag.

For all runs the expected profit per candidate price (the target the algorithm is
estimating) is derived in [Mathematical Model](05-mathematical-model.md).

---

## Run A - Watching the learning happen (40 rounds, epsilon = 0.3, verbose)

Inputs: `base=100, rounds=40, demand=0.6, competitor=100, inventory=100000,`
`epsilon=0.3, customers=100, cost=0.6`. A high `epsilon` and verbose output make
the explore/exploit mechanics visible.

```
=== Running simulation over 40 rounds (epsilon = 0.3) ===
Round     1 | EXPLOIT | price 90.00 | sold   47 | reward 1410.00 | Q 1410.00
Round     2 | EXPLORE | price 105.00 | sold   21 | reward 945.00 | Q 945.00
Round     3 | EXPLOIT | price 90.00 | sold   48 | reward 1440.00 | Q 1425.00
Round     4 | EXPLORE | price 100.00 | sold   23 | reward 920.00 | Q 920.00
Round     5 | EXPLORE | price 90.00 | sold   43 | reward 1290.00 | Q 1380.00
...
Round    18 | EXPLORE | price 95.00 | sold   36 | reward 1260.00 | Q 1260.00
Round    22 | EXPLORE | price 95.00 | sold   39 | reward 1365.00 | Q 1312.50
Round    24 | EXPLOIT | price 95.00 | sold   40 | reward 1400.00 | Q 1341.67
...
Round    40 | EXPLORE | price 100.00 | sold   26 | reward 1040.00 | Q 992.00

==============================================================
                   FINAL SIMULATION REPORT
==============================================================
       Price  Times used    Avg reward    Total reward
--------------------------------------------------------------
       90.00          24       1287.50        30900.00   <- chosen (max Q)
       95.00           6       1213.33         7280.00   <- optimal
      100.00           5        992.00         4960.00
      105.00           3       1035.00         3105.00
      110.00           2        825.00         1650.00
--------------------------------------------------------------
Best-performing price (highest average reward): 90.00
Optimal price (highest expected profit)       : 95.00
Total profit earned                           : 47895.00
Cumulative regret                             : 2394.76
```

**What to notice.**
- `EXPLORE` lines pick prices at random (105, 100, 95, 110), while `EXPLOIT`
  lines pick the current best. The `Q` column shows each price's running mean.
- All five prices get sampled thanks to exploration.
- The engine currently prefers 90, but the *true* optimum is 95. Here they are
  nearly tied in expectation (1242 vs 1257, a ~1% gap), and after only 40 noisy
  rounds the sample means cannot yet separate them. This is exactly why more
  rounds are needed - see Run B.

---

## Run B - Convergence on the defaults (2000 rounds)

Inputs: `base=100, rounds=2000, demand=0.6, competitor=100, inventory=100000,`
`epsilon=0.1, customers=100, cost=0.6`.

```
=== Running simulation over 2000 rounds (epsilon = 0.1) ===
  Round    200 | current best price: 95.00 (avg reward 1281.50)
  Round    400 | current best price: 95.00 (avg reward 1277.34)
  ...
  Round   2000 | current best price: 95.00 (avg reward 1254.95)

==============================================================
                   FINAL SIMULATION REPORT
==============================================================
       Price  Times used    Avg reward    Total reward
--------------------------------------------------------------
       90.00          87       1226.90       106740.00
       95.00        1782       1254.95      2236325.00   <- chosen (max Q) / optimal
      100.00          51       1220.39        62240.00
      105.00          35       1104.43        38655.00
      110.00          45        958.89        43150.00
--------------------------------------------------------------
Best-performing price (highest average reward): 95.00
Optimal price (highest expected profit)       : 95.00
Total profit earned                           : 2487110.00
Cumulative regret                             : 27378.17
```

**What to notice.**
- With enough rounds the estimates separate and the engine correctly identifies
  **95** as optimal, charging it in **1782 / 2000** rounds (~89%, matching
  `1 - epsilon` exploitation plus its share of exploration).
- The average rewards now rank the prices in the correct order and closely match
  the theoretical expectations from the [Mathematical Model](05-mathematical-model.md)
  (1227, 1255, 1220, 1104, 959 vs 1242, 1257, 1200, 1082, 930).
- Regret (27,378) is about **1.1%** of total profit (2,487,110): the price of
  learning is small and shrinking as a fraction of profit.

---

## Run C - A different optimum (competitor = 130, demand = 0.7, 3000 rounds)

Inputs: `base=100, rounds=3000, demand=0.7, competitor=130, inventory=500000,`
`epsilon=0.1, customers=100, cost=0.6`. A higher competitor price makes higher
prices more attractive, shifting the optimum upward.

```
=== Running simulation over 3000 rounds (epsilon = 0.1) ===
  Round    300 | current best price: 110.00 (avg reward 2716.14)
  ...
  Round   3000 | current best price: 110.00 (avg reward 2704.86)

==============================================================
                   FINAL SIMULATION REPORT
==============================================================
       Price  Times used    Avg reward    Total reward
--------------------------------------------------------------
       90.00          94       1934.68       181860.00
       95.00          71       2182.32       154945.00
      100.00          71       2420.28       171840.00
      105.00          56       2565.00       143640.00
      110.00        2708       2704.86      7324750.00   <- chosen (max Q) / optimal
--------------------------------------------------------------
Best-performing price (highest average reward): 110.00
Optimal price (highest expected profit)       : 110.00
Total profit earned                           : 7977035.00
Cumulative regret                             : 149473.38
```

**What to notice.**
- The optimum has moved to **110**, and the engine finds and exploits it (2708 /
  3000 rounds).
- The average rewards are now **monotonically increasing** in price
  (1935 < 2182 < 2420 < 2565 < 2705), the correct ordering for this market.
- Regret (149,473) is under **1.9%** of total profit - the algorithm adapts to a
  completely different demand regime with no code changes.

---

## Run D - Inventory as a hard constraint (inventory = 20000, 1500 rounds)

Inputs: `base=100, rounds=1500, demand=0.6, competitor=100, inventory=20000,`
`epsilon=0.1, customers=100, cost=0.6`.

```
==============================================================
                   FINAL SIMULATION REPORT
==============================================================
       Price  Times used    Avg reward    Total reward
--------------------------------------------------------------
       90.00         140        469.50        65730.00
       95.00        1247        471.36       587790.00   <- chosen (max Q) / optimal
      100.00          45        431.11        19400.00
      105.00          26        252.69         6570.00
      110.00          42        457.14        19200.00
--------------------------------------------------------------
Best-performing price (highest average reward): 95.00
Optimal price (highest expected profit)       : 95.00
Total profit earned                           : 698690.00
Cumulative regret                             : 1187176.13
```

**What to notice.**
- The engine still identifies the correct price (**95**), but demand exhausts the
  limited inventory partway through the horizon. Once stock runs out, later rounds
  sell nothing, dragging the average reward down and inflating regret.
- This illustrates a modelling nuance: regret is measured against an
  *inventory-unaware* expected optimum, so a stock-out shows up as large regret.
  Handling finite inventory explicitly (e.g. pricing to ration scarce stock) is a
  natural extension discussed in
  [Future Work](10-advantages-limitations-future.md).

---

## Takeaways across the runs

1. **Short horizons are noise-dominated** (Run A): near-tied prices cannot be
   separated from few samples.
2. **Given enough rounds the engine converges** to the true optimum and exploits
   it about `1 - epsilon` of the time (Runs B, C).
3. **It adapts to the market**: changing demand or competitor price moves the
   optimum, and the engine follows without any code change (Run C).
4. **Regret stays a small fraction of profit** in the unconstrained regime, and
   the constrained regime (Run D) motivates inventory-aware extensions.
