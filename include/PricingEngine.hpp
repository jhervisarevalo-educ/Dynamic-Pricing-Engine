#ifndef DYNAMIC_PRICING_ENGINE_PRICING_ENGINE_HPP
#define DYNAMIC_PRICING_ENGINE_PRICING_ENGINE_HPP

#include <cstddef>
#include <iosfwd>
#include <vector>

#include "EpsilonGreedyBandit.hpp"
#include "MarketSimulator.hpp"
#include "SimulationConfig.hpp"

/**
 * @file PricingEngine.hpp
 * @brief High-level orchestrator that ties the bandit to the market.
 *
 * The engine is responsible for the domain-specific concerns:
 *   - Generating the candidate prices from the base price.
 *   - Driving the round-by-round learning loop (select -> simulate -> update).
 *   - Tracking aggregate statistics (total profit, cumulative regret).
 *   - Rendering per-round and final reports to an output stream.
 *
 * It delegates *what to try next* to EpsilonGreedyBandit and *what happens when
 * we try it* to MarketSimulator, keeping each component focused and testable.
 */
class PricingEngine {
public:
    /// Aggregate results produced by a completed simulation run.
    struct Report {
        std::vector<PriceArm> arms;      ///< Final per-price statistics.
        double totalProfit = 0.0;        ///< Sum of profit over all rounds.
        double cumulativeRegret = 0.0;   ///< Total regret versus the optimum.
        std::size_t bestArmIndex = 0;    ///< Arm the engine settled on (max Q).
        std::size_t optimalArmIndex = 0; ///< True optimum (max expected profit).
    };

    /// Construct an engine for a given configuration.
    explicit PricingEngine(const SimulationConfig& config);

    /**
     * @brief Run the full simulation.
     * @param out            Stream for progress output.
     * @param verbose        When true, print a line for every round; otherwise
     *                       print periodic progress snapshots only.
     * @return The final aggregated report.
     */
    Report run(std::ostream& out, bool verbose);

    /// The candidate prices generated from the base price.
    const std::vector<double>& candidatePrices() const { return candidatePrices_; }

    /// Print a formatted final report to the given stream.
    void printFinalReport(std::ostream& out, const Report& report) const;

private:
    /// Build the candidate prices (-10%, -5%, base, +5%, +10%) from the config.
    void generateCandidatePrices();

    /// Print a single round's outcome.
    void printRoundLine(std::ostream& out,
                        std::size_t round,
                        std::size_t armIndex,
                        bool wasExploration,
                        const MarketSimulator::RoundOutcome& outcome,
                        const EpsilonGreedyBandit& bandit) const;

    SimulationConfig config_;                ///< Run configuration.
    std::vector<double> candidatePrices_;    ///< Generated candidate prices.
    MarketSimulator market_;                 ///< Demand / profit environment.
};

#endif  // DYNAMIC_PRICING_ENGINE_PRICING_ENGINE_HPP
