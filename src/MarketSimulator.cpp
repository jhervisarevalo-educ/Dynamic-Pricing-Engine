#include "MarketSimulator.hpp"

#include <algorithm>
#include <cmath>

/**
 * @file MarketSimulator.cpp
 * @brief Implementation of the demand / profit environment.
 */

namespace {
/// Steepness of the logistic demand curve. Larger values make demand more
/// sensitive to the gap between the offered price and the competitor price.
constexpr double kPriceSensitivity = 8.0;
}  // namespace

MarketSimulator::MarketSimulator(const SimulationConfig& config)
    : config_(config),
      remainingInventory_(config.inventoryLevel),
      // Offset the seed so the environment's randomness is decorrelated from
      // the bandit's exploration randomness while remaining reproducible.
      rng_(config.seed + 0x9E3779B9u) {}

double MarketSimulator::purchaseProbability(double price) const {
    // Relative price gap versus the competitor. Positive when we are more
    // expensive than the competitor, negative when we are cheaper.
    const double reference = (config_.competitorPrice > 0.0)
                                 ? config_.competitorPrice
                                 : config_.basePrice;
    const double relativeGap = (price - reference) / reference;

    // Logistic curve: 0.5 at the competitor price, higher when cheaper.
    const double logistic = 1.0 / (1.0 + std::exp(kPriceSensitivity * relativeGap));

    // Scale by the overall demand level and clamp to a valid probability.
    const double probability = config_.demandLevel * logistic;
    return std::clamp(probability, 0.0, 1.0);
}

double MarketSimulator::expectedProfit(double price) const {
    const double marginPerUnit = price - config_.unitCost();
    const double expectedUnits =
        static_cast<double>(config_.customersPerRound) * purchaseProbability(price);
    return expectedUnits * marginPerUnit;
}

MarketSimulator::RoundOutcome MarketSimulator::simulateRound(double price) {
    RoundOutcome outcome;

    const double probability = purchaseProbability(price);
    std::bernoulli_distribution buys(probability);

    // Each arriving customer independently decides whether to purchase.
    std::size_t unitsSold = 0;
    for (std::size_t c = 0; c < config_.customersPerRound; ++c) {
        if (remainingInventory_ == 0) {
            break;  // Nothing left to sell.
        }
        if (buys(rng_)) {
            ++unitsSold;
            --remainingInventory_;
        }
    }

    const double marginPerUnit = price - config_.unitCost();
    outcome.unitsSold = unitsSold;
    outcome.profit = static_cast<double>(unitsSold) * marginPerUnit;
    return outcome;
}

void MarketSimulator::reset() {
    remainingInventory_ = config_.inventoryLevel;
}
