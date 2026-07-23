# 2. Problem Statement

## 2.1 Informal statement

A retailer sells a single product and may choose from a small set of candidate
prices. Each price yields an *unknown* and *noisy* profit that depends on how
many customers decide to buy at that price. The retailer wants to discover, and
then consistently charge, the price that maximises total profit over a selling
horizon, while wasting as little profit as possible on unprofitable experiments.

## 2.2 Formal statement

Let there be a fixed set of `k` candidate prices:

```
P = { p_1, p_2, ..., p_k }
```

Time proceeds in discrete **rounds** `t = 1, 2, ..., n`. In each round the engine
selects one price `p_{a_t}` (equivalently, one *arm* `a_t in {1,...,k}`) and
receives a random **reward** (profit):

```
R_t = f(p_{a_t}) + noise
```

Each arm `i` has an unknown expected reward:

```
q*(i) = E[ R_t | a_t = i ]
```

The optimal arm is the one with the greatest expected reward:

```
a* = argmax_i q*(i)          q* = max_i q*(i)
```

### Objective

Maximise the expected total reward over the horizon:

```
maximise   E[ sum_{t=1..n} R_t ]
```

Equivalently, minimise the **cumulative regret**, i.e. the profit lost by not
always playing the optimal arm:

```
Regret(n) = sum_{t=1..n} ( q* - q*(a_t) )
```

## 2.3 Why this is hard: the exploration/exploitation dilemma

The expected rewards `q*(i)` are unknown and can only be estimated from the noisy
rewards actually observed. This creates a fundamental tension:

- If the engine always plays the arm that *currently* looks best
  (pure exploitation), it may lock onto a suboptimal price whose early rewards
  happened to look good, never discovering a better one.
- If the engine keeps trying every arm (pure exploration), it learns the true
  values well but sacrifices profit on prices it already knows are poor.

A good policy must **balance** the two. The epsilon-Greedy algorithm resolves
this by exploiting most of the time while occasionally exploring at random. See
[Algorithm Explanation](04-algorithm-explanation.md).

## 2.4 Assumptions and scope

- The candidate price set is **fixed** and **small** (`k = 5` by default). This
  is what lets us treat `k` as a constant in the complexity analysis.
- Rewards are **stationary**: the demand behaviour does not change over time.
  (Handling non-stationary demand is discussed in
  [Future Work](10-advantages-limitations-future.md).)
- Rounds are **independent**: the outcome of one round does not influence the
  demand distribution of the next (aside from inventory being consumed).
- A single product is priced in isolation (no cross-product effects).
