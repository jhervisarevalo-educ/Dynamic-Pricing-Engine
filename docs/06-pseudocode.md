# 6. Pseudocode

## 6.1 Top-level simulation

```
INPUT:  base price b, rounds n, demand level d, competitor price p_comp,
        inventory I, exploration rate epsilon
OUTPUT: per-price statistics, total profit, best price, cumulative regret

# --- Setup ---
prices  <- { b*(1-0.10), b*(1-0.05), b, b*(1+0.05), b*(1+0.10) }   # k prices
for each price p_i in prices:
    n_i <- 0            # pull count
    Q_i <- 0            # mean-reward estimate
    S_i <- 0            # total reward (for reporting)

optimalExpected <- max_i expectedProfit(p_i)      # for regret only
totalProfit     <- 0
cumulativeRegret <- 0

# --- Learning loop ---   (n iterations)
for t <- 1 to n:
    a <- SELECT_ARM(epsilon, Q, k)                # explore or exploit
    R <- SIMULATE_ROUND(p_a, d, p_comp, I)        # noisy profit
    UPDATE_ARM(a, R)                              # incremental average
    totalProfit      <- totalProfit + R
    cumulativeRegret <- cumulativeRegret + (optimalExpected - R)
    REPORT(t, a, R, Q)

# --- Final report ---
best <- argmax_i Q_i
PRINT n_i, Q_i for all i; totalProfit; prices[best]; cumulativeRegret
```

## 6.2 Action selection (epsilon-Greedy)

```
function SELECT_ARM(epsilon, Q, k):
    u <- uniform_random[0, 1)
    if u < epsilon:                 # EXPLORE
        return uniform_random_integer in {1, ..., k}
    else:                           # EXPLOIT
        return ARGMAX_i Q_i         # ties -> lowest index
```

## 6.3 Reward update (incremental average)

```
function UPDATE_ARM(a, R):
    n_a <- n_a + 1
    S_a <- S_a + R
    Q_a <- Q_a + (1 / n_a) * (R - Q_a)
```

## 6.4 Market simulation (the environment)

```
function SIMULATE_ROUND(p, d, p_comp, I):
    prob <- d / (1 + exp(K * (p - p_comp)/p_comp))   # clamp to [0,1]
    sold <- 0
    for customer <- 1 to m:                          # m customers this round
        if remainingInventory == 0: break
        if uniform_random[0,1) < prob:
            sold <- sold + 1
            remainingInventory <- remainingInventory - 1
    return (p - c) * sold                            # profit; c = unit cost
```

## 6.5 Expected profit (used only for regret / optimum)

```
function expectedProfit(p):
    prob <- d / (1 + exp(K * (p - p_comp)/p_comp))   # clamp to [0,1]
    return m * prob * (p - c)
```

The correspondence between this pseudocode and the C++ source is direct:
`SELECT_ARM`/`UPDATE_ARM` live in
[`EpsilonGreedyBandit.cpp`](../src/EpsilonGreedyBandit.cpp),
`SIMULATE_ROUND`/`expectedProfit` in
[`MarketSimulator.cpp`](../src/MarketSimulator.cpp), and the learning loop in
[`PricingEngine.cpp`](../src/PricingEngine.cpp).
