# 10. Advantages, Limitations, and Future Work

## 10.1 Why epsilon-Greedy is appropriate for dynamic pricing

- **Online learning without a model.** Prices must be chosen before demand is
  known. epsilon-Greedy learns purely from observed profit, requiring no prior
  demand curve, no training data set, and no offline model fitting.
- **Directly optimises the business metric.** The reward *is* profit, so the
  algorithm optimises exactly what the retailer cares about, rather than a proxy.
- **Built-in exploration/exploitation balance.** Pricing is a textbook
  explore/exploit problem: you must test prices to learn, but every test on a bad
  price costs real money. epsilon-Greedy handles this trade-off with a single,
  interpretable parameter.
- **Cheap and real-time.** `O(k)` work per decision and `O(k)` memory (see
  [Complexity](08-complexity-analysis.md)) make it suitable for high-frequency
  repricing.
- **Robust and simple.** It has no fragile hyperparameters beyond `epsilon`, is
  easy to reason about, and degrades gracefully.

## 10.2 Advantages (general)

- Extremely simple to implement and explain.
- Provably converges to the optimal arm (see
  [Correctness](09-correctness-and-convergence.md)).
- Constant-time, constant-memory updates via the incremental average.
- No external libraries; fully deterministic/reproducible when seeded.
- Cleanly modular: the policy, the environment, and the orchestration are
  independent and independently testable.

## 10.3 Limitations

- **Constant exploration cost.** With fixed `epsilon`, the engine keeps spending
  a small fraction of rounds on known-bad prices forever, yielding (small) linear
  regret. Smarter policies achieve logarithmic regret.
- **Undirected exploration.** Exploration is uniform: an obviously terrible price
  is explored just as often as a plausible runner-up, which is wasteful.
- **Assumes stationarity.** If demand shifts over time (seasonality, trends), a
  simple running mean is slow to forget stale data and adapt.
- **Fixed, discrete price grid.** Only the five predefined prices can ever be
  chosen; the true optimum may lie between grid points.
- **Ignores context.** It cannot use features such as time of day, customer
  segment, or weather to condition its price.
- **Single product.** Cross-product effects (substitutes/complements) are not
  modelled.

## 10.4 Possible future improvements

- **Decaying epsilon** (e.g. `epsilon_t = min(1, c/t)`) to explore aggressively
  early and exploit later, achieving logarithmic regret.
- **Optimistic initialisation** of `Q` to encourage early exploration without an
  explicit random branch.
- **UCB1** (Upper Confidence Bound): pick `argmax_i (Q_i + sqrt(2 ln t / n_i))`,
  giving directed exploration and provable logarithmic regret.
- **Thompson Sampling**: maintain a posterior per arm and sample from it,
  typically excellent empirical performance.
- **Non-stationary handling**: replace the plain mean with an exponentially
  weighted / sliding-window average, or use discounted rewards.
- **Contextual bandits**: condition the price on observed features (LinUCB,
  contextual Thompson sampling).
- **Continuous / adaptive price grid**: refine the grid around the current best
  price, or use a continuum-armed bandit.
- **Richer demand model**: incorporate price elasticity estimation, reference
  prices, and competitor dynamics.
- **Statistical guarantees in output**: report confidence intervals on each
  arm's estimate alongside the point estimate.
