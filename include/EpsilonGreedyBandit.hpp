#ifndef DYNAMIC_PRICING_ENGINE_EPSILON_GREEDY_BANDIT_HPP
#define DYNAMIC_PRICING_ENGINE_EPSILON_GREEDY_BANDIT_HPP

#include <cstddef>
#include <cstdint>
#include <random>
#include <vector>

#include "PriceArm.hpp"

/**
 * @file EpsilonGreedyBandit.hpp
 * @brief The epsilon-Greedy Multi-Armed Bandit policy.
 *
 * This class implements the decision-making core of the pricing engine. It is
 * intentionally decoupled from the pricing domain: it only knows about a set of
 * abstract "arms", each exposing a reward estimate. This separation keeps the
 * algorithm reusable and easy to test.
 *
 * Responsibilities:
 *   - Action selection (selectArm): choose which arm to pull using the
 *     epsilon-Greedy rule (explore with probability epsilon, otherwise exploit).
 *   - Reward updating (updateArm): fold a newly observed reward into an arm's
 *     running mean estimate using the incremental-average formula.
 *
 * The random number generator is owned by the instance (no global state), which
 * makes runs reproducible when seeded deterministically.
 */
class EpsilonGreedyBandit {
public:
    /**
     * @param arms     Candidate arms to manage. Copied into the bandit so the
     *                 bandit owns and updates its own statistics.
     * @param epsilon  Exploration probability in [0, 1].
     * @param seed     Seed for the internal Mersenne Twister engine.
     */
    EpsilonGreedyBandit(std::vector<PriceArm> arms,
                        double epsilon,
                        std::uint_fast32_t seed);

    /**
     * @brief Select an arm to pull for the current round.
     *
     * With probability epsilon this performs *exploration*: an arm is chosen
     * uniformly at random from all arms. Otherwise it performs *exploitation*:
     * the arm with the highest current reward estimate (Q) is chosen. Ties are
     * broken by the lowest index for determinism.
     *
     * @param wasExploration Optional out-parameter set to true when the choice
     *                       was an exploration step, false for exploitation.
     * @return Index of the selected arm.
     */
    std::size_t selectArm(bool* wasExploration = nullptr);

    /**
     * @brief Incorporate an observed reward for a pulled arm.
     *
     * Uses the incremental-average update, which maintains the exact mean of all
     * rewards seen for the arm without storing them:
     *     n      <- n + 1
     *     Q_new  <- Q_old + (1 / n) * (reward - Q_old)
     *
     * @param armIndex Index of the arm that was pulled.
     * @param reward   Observed reward (profit) for this pull.
     */
    void updateArm(std::size_t armIndex, double reward);

    /// Index of the arm with the highest current reward estimate.
    std::size_t bestArmIndex() const;

    /// Read-only access to the managed arms and their statistics.
    const std::vector<PriceArm>& arms() const { return arms_; }

    /// Number of arms managed by the bandit.
    std::size_t armCount() const { return arms_.size(); }

private:
    std::vector<PriceArm> arms_;  ///< Managed arms and their live statistics.
    double epsilon_;              ///< Exploration probability.

    // The bandit owns its RNG so there is no shared/global mutable state.
    std::mt19937 rng_;                                   ///< PRNG engine.
    std::uniform_real_distribution<double> uniform01_;   ///< Draws in [0, 1).
};

#endif  // DYNAMIC_PRICING_ENGINE_EPSILON_GREEDY_BANDIT_HPP
