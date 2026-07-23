#ifndef DYNAMIC_PRICING_ENGINE_SIMULATION_CONFIG_HPP
#define DYNAMIC_PRICING_ENGINE_SIMULATION_CONFIG_HPP

#include <cstddef>
#include <cstdint>

/**
 * @file SimulationConfig.hpp
 * @brief All user-provided and derived parameters for one simulation run.
 *
 * Grouping the parameters in a single struct keeps function signatures small,
 * makes the configuration easy to pass around, and avoids any reliance on
 * global variables.
 */
struct SimulationConfig {
    // ----- User-provided inputs -------------------------------------------

    /// Reference ("list") price of the product. Candidate prices are derived
    /// from this value (e.g. -10%, -5%, 0%, +5%, +10%).
    double basePrice = 100.0;

    /// Number of pricing rounds to simulate. Each round represents a batch of
    /// customers exposed to a single chosen price. Denoted n.
    std::size_t numRounds = 1000;

    /// Demand level in the range (0, 1]. Higher values mean customers are more
    /// willing to buy at a given price (larger market / stronger demand).
    double demandLevel = 0.5;

    /// The competitor's selling price. Prices well above the competitor reduce
    /// the probability of purchase; prices below it increase that probability.
    double competitorPrice = 100.0;

    /// Units available to sell across the entire simulation. Acts as an upper
    /// bound (cap) on how many units may be sold each round.
    std::size_t inventoryLevel = 100000;

    // ----- Algorithm / model parameters (sensible defaults) ---------------

    /// Exploration probability for the epsilon-Greedy policy, in [0, 1].
    /// With probability epsilon the engine explores a random price; otherwise
    /// it exploits the current best-known price.
    double epsilon = 0.1;

    /// Per-round customer traffic: how many customers see the offered price in
    /// a single round. Each customer independently decides to buy or not.
    std::size_t customersPerRound = 100;

    /// Unit cost expressed as a fraction of the base price. Profit per unit is
    /// (price - unitCost). Defaults to 60% of the base price.
    double costFraction = 0.6;

    /// Seed for the pseudo-random number generator. Using a fixed seed makes a
    /// run fully reproducible; callers may randomize it if desired.
    std::uint_fast32_t seed = 42u;

    // ----- Derived helpers -------------------------------------------------

    /// Absolute unit cost derived from the base price and cost fraction.
    double unitCost() const { return basePrice * costFraction; }
};

#endif  // DYNAMIC_PRICING_ENGINE_SIMULATION_CONFIG_HPP
