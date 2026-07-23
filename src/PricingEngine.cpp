#include "PricingEngine.hpp"

#include <iomanip>
#include <ios>
#include <ostream>

/**
 * @file PricingEngine.cpp
 * @brief Implementation of the pricing engine orchestration and reporting.
 */

namespace {
/// Percentage offsets applied to the base price to build candidate prices.
/// Fixed at five options, so k = 5 in the complexity analysis.
constexpr double kPriceOffsets[] = {-0.10, -0.05, 0.0, 0.05, 0.10};

/// Helper: print a fixed-precision currency value.
std::ostream& money(std::ostream& out) {
    return out << std::fixed << std::setprecision(2);
}
}  // namespace

PricingEngine::PricingEngine(const SimulationConfig& config)
    : config_(config), market_(config) {
    generateCandidatePrices();
}

void PricingEngine::generateCandidatePrices() {
    candidatePrices_.clear();
    for (double offset : kPriceOffsets) {
        candidatePrices_.push_back(config_.basePrice * (1.0 + offset));
    }
}

PricingEngine::Report PricingEngine::run(std::ostream& out, bool verbose) {
    // ----- Build the arms, one per candidate price ------------------------
    std::vector<PriceArm> arms;
    arms.reserve(candidatePrices_.size());
    for (double price : candidatePrices_) {
        arms.emplace_back(price);
    }

    EpsilonGreedyBandit bandit(std::move(arms), config_.epsilon, config_.seed);
    market_.reset();

    // ----- Determine the theoretically optimal price (for regret) ---------
    Report report;
    report.optimalArmIndex = 0;
    double bestExpected = market_.expectedProfit(candidatePrices_[0]);
    for (std::size_t i = 1; i < candidatePrices_.size(); ++i) {
        const double expected = market_.expectedProfit(candidatePrices_[i]);
        if (expected > bestExpected) {
            bestExpected = expected;
            report.optimalArmIndex = i;
        }
    }
    const double optimalExpectedProfit = bestExpected;

    // ----- Progress reporting cadence -------------------------------------
    // In non-verbose mode, print roughly ten progress snapshots.
    const std::size_t snapshotEvery =
        (config_.numRounds >= 10) ? (config_.numRounds / 10) : 1;

    out << "\n=== Running simulation over " << config_.numRounds
        << " rounds (epsilon = " << config_.epsilon << ") ===\n";

    // ----- Main learning loop: O(n * k) -----------------------------------
    for (std::size_t round = 1; round <= config_.numRounds; ++round) {
        // 1. Action selection (explore vs exploit).
        bool wasExploration = false;
        const std::size_t armIndex = bandit.selectArm(&wasExploration);
        const double price = bandit.arms()[armIndex].price;

        // 2. Observe the environment's reward for this action.
        const MarketSimulator::RoundOutcome outcome = market_.simulateRound(price);

        // 3. Update the reward estimate for the chosen arm.
        bandit.updateArm(armIndex, outcome.profit);

        // 4. Bookkeeping: profit and regret.
        report.totalProfit += outcome.profit;
        report.cumulativeRegret += (optimalExpectedProfit - outcome.profit);

        // 5. Reporting.
        if (verbose) {
            printRoundLine(out, round, armIndex, wasExploration, outcome, bandit);
        } else if (round % snapshotEvery == 0 || round == config_.numRounds) {
            const std::size_t currentBest = bandit.bestArmIndex();
            out << "  Round " << std::setw(6) << round
                << " | current best price: " << money
                << bandit.arms()[currentBest].price
                << " (avg reward " << bandit.arms()[currentBest].estimatedReward
                << ")\n";
            out.unsetf(std::ios::fixed);
        }
    }

    // ----- Finalize the report --------------------------------------------
    report.arms = bandit.arms();
    report.bestArmIndex = bandit.bestArmIndex();
    return report;
}

void PricingEngine::printRoundLine(std::ostream& out,
                                   std::size_t round,
                                   std::size_t armIndex,
                                   bool wasExploration,
                                   const MarketSimulator::RoundOutcome& outcome,
                                   const EpsilonGreedyBandit& bandit) const {
    out << "Round " << std::setw(5) << round << " | "
        << (wasExploration ? "EXPLORE" : "EXPLOIT") << " | "
        << "price " << money << bandit.arms()[armIndex].price << " | "
        << "sold " << std::setw(4) << outcome.unitsSold << " | "
        << "reward " << money << outcome.profit << " | "
        << "Q " << money << bandit.arms()[armIndex].estimatedReward << "\n";
    out.unsetf(std::ios::fixed);
}

void PricingEngine::printFinalReport(std::ostream& out, const Report& report) const {
    // Fix precision for all numeric output in this report.
    out << std::fixed << std::setprecision(2);

    out << "\n==============================================================\n";
    out << "                   FINAL SIMULATION REPORT\n";
    out << "==============================================================\n";

    // Column header.
    out << std::right
        << std::setw(12) << "Price"
        << std::setw(12) << "Times used"
        << std::setw(14) << "Avg reward"
        << std::setw(16) << "Total reward"
        << "\n";
    out << "--------------------------------------------------------------\n";

    // One row per candidate price.
    for (std::size_t i = 0; i < report.arms.size(); ++i) {
        const PriceArm& arm = report.arms[i];
        out << std::right
            << std::setw(12) << arm.price
            << std::setw(12) << arm.pullCount
            << std::setw(14) << arm.estimatedReward
            << std::setw(16) << arm.totalReward;

        if (i == report.bestArmIndex) {
            out << "   <- chosen (max Q)";
        }
        if (i == report.optimalArmIndex) {
            out << (i == report.bestArmIndex ? " / optimal" : "   <- optimal");
        }
        out << "\n";
    }

    out << "--------------------------------------------------------------\n";
    out << "Best-performing price (highest average reward): "
        << report.arms[report.bestArmIndex].price << "\n";
    out << "Optimal price (highest expected profit)       : "
        << report.arms[report.optimalArmIndex].price << "\n";
    out << "Total profit earned                           : "
        << report.totalProfit << "\n";
    out << "Cumulative regret                             : "
        << report.cumulativeRegret << "\n";
    out << "==============================================================\n";

    out.unsetf(std::ios::fixed);
}
