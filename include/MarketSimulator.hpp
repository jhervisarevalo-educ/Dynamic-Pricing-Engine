#ifndef DYNAMIC_PRICING_ENGINE_MARKET_SIMULATOR_HPP
#define DYNAMIC_PRICING_ENGINE_MARKET_SIMULATOR_HPP

#include <cstddef>
#include <random>

#include "SimulationConfig.hpp"

/**
 * @file MarketSimulator.hpp
 * @brief Simulates customer demand and the profit earned at a given price.
 *
 * The simulator embodies the (unknown, from the algorithm's point of view)
 * environment that the bandit is learning about. For a chosen price it:
 *   1. Computes a per-customer purchase probability from a demand curve that
 *      depends on the price, the overall demand level and the competitor price.
 *   2. Draws independent Bernoulli "buy / no-buy" decisions for the customers
 *      that arrive during the round.
 *   3. Caps the number of units sold by the remaining inventory.
 *   4. Returns the units sold and the profit (the bandit's reward).
 *
 * The simulator owns its own random engine so the environment's stochasticity
 * is independent of, and does not perturb, the bandit's exploration draws.
 */
class MarketSimulator {
public:
    /// Result of simulating a single pricing round.
    struct RoundOutcome {
        std::size_t unitsSold = 0;  ///< Units actually sold this round.
        double profit = 0.0;        ///< Profit earned this round (the reward).
    };

    /// Construct the simulator from the run configuration.
    explicit MarketSimulator(const SimulationConfig& config);

    /**
     * @brief Simulate one round at the given price.
     * @param price The selling price offered this round.
     * @return Units sold and profit, respecting the remaining inventory.
     */
    RoundOutcome simulateRound(double price);

    /**
     * @brief Probability that a single customer buys at the given price.
     *
     * A logistic demand curve centred on the competitor price: at the
     * competitor price the (pre-demand-scaling) probability is 0.5; it rises as
     * the price drops below the competitor and falls as it rises above. The
     * whole curve is scaled by the demand level.
     */
    double purchaseProbability(double price) const;

    /**
     * @brief Expected (noise-free) profit per round at the given price.
     *
     * Equal to customersPerRound * purchaseProbability(price) * marginPerUnit.
     * This ignores inventory depletion and randomness and is used to identify
     * the theoretically optimal price and to compute regret.
     */
    double expectedProfit(double price) const;

    /// Units still available to sell in the current simulation.
    std::size_t remainingInventory() const { return remainingInventory_; }

    /// Restore inventory to its initial level (for repeated simulations).
    void reset();

private:
    SimulationConfig config_;             ///< Copy of the run configuration.
    std::size_t remainingInventory_;      ///< Mutable inventory counter.
    std::mt19937 rng_;                    ///< Environment PRNG (own state).
};

#endif  // DYNAMIC_PRICING_ENGINE_MARKET_SIMULATOR_HPP
