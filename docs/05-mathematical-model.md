# 5. Mathematical Model

This document formalises the reward, the demand curve used by the simulated
market, the incremental-average estimator, and the regret measure.

## 5.1 Candidate prices

Given a base price `b`, the engine generates `k = 5` candidate prices by applying
fixed percentage offsets:

```
p_i = b * (1 + delta_i),   delta in { -0.10, -0.05, 0.00, +0.05, +0.10 }
```

## 5.2 Profit (reward) per round

Let `c` be the unit cost. By default `c = 0.6 * b` (60% of the base price), so the
margin per unit at price `p` is `(p - c)`. If `s` units are sold in a round, the
round's profit (reward) is:

```
R = (p - c) * s
```

## 5.3 Demand model

Each round, `m` customers (`customersPerRound`) are exposed to the offered price
`p`. Each customer independently buys with probability `Pr_buy(p)`, giving a
Binomial number of sales `s ~ Binomial(m, Pr_buy(p))`, further capped by the
remaining inventory.

The purchase probability uses a **logistic demand curve** centred on the
competitor price `p_comp`, scaled by the demand level `d in (0, 1]`:

```
                          d
Pr_buy(p) = --------------------------------
             1 + exp( K * (p - p_comp)/p_comp )
```

where `K` is a price-sensitivity constant (`K = 8` in the code). Properties:

- At `p = p_comp` the logistic term is `0.5`, so `Pr_buy = d/2`.
- Lowering the price below the competitor **raises** the probability toward `d`.
- Raising it above the competitor **lowers** the probability toward `0`.
- Higher demand level `d` scales the whole curve up.

The result is clamped to `[0, 1]`.

## 5.4 Expected reward of an arm

Ignoring inventory limits, the expected profit per round at price `p` is:

```
q*(p) = E[R | price = p] = m * Pr_buy(p) * (p - c)
```

This is the quantity the algorithm is implicitly estimating. It embodies the
core trade-off:

- `Pr_buy(p)` **decreases** in `p` (fewer buyers when expensive).
- The margin `(p - c)` **increases** in `p` (more profit per sale).

Their product has an interior maximum, which is the optimal price `p*`. The code
computes `q*(p)` in `MarketSimulator::expectedProfit` and uses it only to
identify the optimum and to measure regret; the *algorithm itself never sees it*.

### Worked example (defaults: b = 100, p_comp = 100, d = 0.6, c = 60, m = 100, K = 8)

| Price p | (p - p_comp)/p_comp | logistic | Pr_buy = d*logistic | margin | q*(p) = m*Pr_buy*margin |
|--------:|--------------------:|---------:|--------------------:|-------:|------------------------:|
| 90  | -0.05*2 = -0.10 | 0.690 | 0.414 | 30 | 1242 |
| **95**  | -0.05 | 0.599 | 0.359 | 35 | **1257 (max)** |
| 100 | 0.00  | 0.500 | 0.300 | 40 | 1200 |
| 105 | +0.05 | 0.401 | 0.240 | 45 | 1082 |
| 110 | +0.10 | 0.310 | 0.186 | 50 | 930 |

The optimal price is `p* = 95`. The sample runs confirm the engine discovers it.

## 5.5 The incremental-average estimator

Let `R_1, ..., R_n` be the rewards observed for a given arm and let
`Q_n = (1/n) * sum_{j=1..n} R_j` be their mean. Then:

```
Q_n = (1/n) * sum_{j=1..n} R_j
    = (1/n) * ( R_n + sum_{j=1..n-1} R_j )
    = (1/n) * ( R_n + (n-1) * Q_{n-1} )
    = Q_{n-1} + (1/n) * ( R_n - Q_{n-1} )
```

This is exactly the update used in `updateArm`. It proves the incremental form
computes the true arithmetic mean while storing only `Q` and `n` (`O(1)` memory).

## 5.6 Regret

Per-round regret is the expected profit lost by not playing the optimal arm:

```
regret_t = q*(p*) - R_t
```

and the reported **cumulative regret** is:

```
Regret(n) = sum_{t=1..n} ( q*(p*) - R_t )
```

Because exploration keeps costing a little each round, regret keeps growing, but
the *rate* of growth is roughly `epsilon * (average suboptimality gap)` per
round, which is small. Crucially, regret grows far slower than total profit, so
the fraction of profit sacrificed to learning shrinks over time. See
[Correctness & Convergence](09-correctness-and-convergence.md).
