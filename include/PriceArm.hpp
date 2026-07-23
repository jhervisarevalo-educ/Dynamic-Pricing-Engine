#ifndef DYNAMIC_PRICING_ENGINE_PRICE_ARM_HPP
#define DYNAMIC_PRICING_ENGINE_PRICE_ARM_HPP

#include <cstddef>

/**
 * @file PriceArm.hpp
 * @brief Definition of a single "arm" of the Multi-Armed Bandit.
 *
 * In the Multi-Armed Bandit formulation each candidate selling price is one
 * "arm" of the bandit. Pulling an arm means offering that price to a batch of
 * simulated customers for one round and observing the resulting profit
 * (the reward).
 *
 * The struct keeps everything required to maintain an *incremental* estimate
 * of the average reward of the arm, so we never have to store the full history
 * of observed rewards. This keeps the per-arm memory footprint O(1).
 */
struct PriceArm {
    /// The concrete selling price this arm represents (in currency units).
    double price = 0.0;

    /// How many times this arm has been selected (pulled) so far. Denoted n.
    std::size_t pullCount = 0;

    /// Sum of all rewards observed for this arm. Kept for reporting/inspection.
    double totalReward = 0.0;

    /**
     * Current estimate of the arm's mean reward, denoted Q.
     * Updated incrementally after every pull using:
     *     Q_{n} = Q_{n-1} + (1 / n) * (R_n - Q_{n-1})
     */
    double estimatedReward = 0.0;

    /// Construct an arm for a given price. All statistics start at zero.
    PriceArm() = default;
    explicit PriceArm(double p) : price(p) {}
};

#endif  // DYNAMIC_PRICING_ENGINE_PRICE_ARM_HPP
