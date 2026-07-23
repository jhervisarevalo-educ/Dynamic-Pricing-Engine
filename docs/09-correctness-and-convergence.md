# 9. Correctness and Convergence

This document argues two things: (a) the estimator is **correct** (it computes
what it claims to), and (b) the policy **converges** toward charging the optimal
price.

## 9.1 Correctness of the incremental average

**Claim.** After an arm has been pulled `n` times with rewards `R_1,...,R_n`, the
stored estimate equals the true arithmetic mean `Q_n = (1/n) sum_j R_j`.

**Proof by induction on `n`.**

- *Base case* (`n = 1`): starting from `Q_0 = 0`, the update gives
  `Q_1 = Q_0 + (1/1)(R_1 - Q_0) = R_1`, which is the mean of one sample.
- *Inductive step*: assume `Q_{n-1} = (1/(n-1)) sum_{j=1..n-1} R_j`. The update is
  `Q_n = Q_{n-1} + (1/n)(R_n - Q_{n-1})`. Substituting and simplifying:

  ```
  Q_n = ((n-1)*Q_{n-1} + R_n) / n
      = ( sum_{j=1..n-1} R_j + R_n ) / n
      = (1/n) * sum_{j=1..n} R_j
  ```

  which is the mean of `n` samples. QED.

So `estimatedReward` is always the exact mean of the rewards observed so far, and
it uses only `O(1)` memory (see [Complexity](08-complexity-analysis.md)). The
matching code is in `EpsilonGreedyBandit::updateArm`.

## 9.2 Every arm is sampled infinitely often

With `epsilon > 0`, on each round there is probability at least `epsilon/k` of
selecting any specific arm (an exploration step that lands on it). Over an
infinite horizon the expected number of pulls of every arm diverges, so each arm
is pulled infinitely often (this is a standard Borel-Cantelli argument).

## 9.3 Estimates converge to the true means

Rewards for a fixed price are independent and identically distributed with a
finite mean `q*(i)` and finite variance (the reward is a bounded profit times a
Binomial count). Because arm `i` is pulled infinitely often (9.2) and
`Q_i` is the running sample mean (9.1), the **Strong Law of Large Numbers** gives:

```
Q_i  ->  q*(i)   almost surely, as n_i -> infinity, for every arm i
```

## 9.4 The greedy choice converges to the optimal price

Let `a* = argmax_i q*(i)` be the optimal arm, and assume it is unique with a
positive gap `Delta = q*(a*) - max_{i != a*} q*(i) > 0`. By 9.3 all estimates
`Q_i` converge to their true values, so for large enough `n` the ordering of the
estimates matches the ordering of the true means. In particular:

```
Q_{a*} > Q_i   for all i != a*      (eventually, almost surely)
```

Therefore `bestArmIndex()` eventually returns `a*`, and on every exploitation
step (probability `1 - epsilon`) the engine charges the optimal price. This is
exactly the behaviour observed in the [Sample Runs](11-sample-runs.md), where the
optimal price accumulates the overwhelming majority of pulls.

## 9.5 Regret behaviour

The engine is not perfect: on each round it still explores with probability
`epsilon`, and an exploration step picks a suboptimal arm with probability
`(k-1)/k`. Each such step costs on average some suboptimality gap. Hence the
**expected per-round regret** tends to a small constant:

```
E[regret per round]  ->  epsilon * ( (1/k) * sum_{i != a*} (q*(a*) - q*(i)) )
```

and the cumulative regret grows roughly **linearly but with a small slope**
proportional to `epsilon`. Two consequences:

- Total profit grows at rate `~ q*(a*)` per round, while regret grows at the much
  smaller rate above, so the **fraction** of profit lost to learning shrinks
  toward `epsilon * (average gap)/q*(a*)` and stays small.
- Reducing `epsilon` (or decaying it over time) lowers the asymptotic regret
  slope at the cost of slower initial learning. Constant-`epsilon` epsilon-Greedy
  trades a small permanent exploration cost for simplicity and robustness. A
  decaying schedule (e.g. `epsilon_t = 1/t`) can achieve logarithmic regret; see
  [Future Work](10-advantages-limitations-future.md).

## 9.6 Summary

- The estimator is provably the exact running mean (9.1).
- Exploration guarantees every price is tried infinitely often (9.2).
- The Law of Large Numbers drives each estimate to its true value (9.3).
- Consequently the greedy price converges to the optimal price (9.4).
- Regret grows slowly and predictably, controlled by `epsilon` (9.5).
