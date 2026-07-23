#include "EpsilonGreedyBandit.hpp"

#include <stdexcept>
#include <utility>

/**
 * @file EpsilonGreedyBandit.cpp
 * @brief Implementation of the epsilon-Greedy Multi-Armed Bandit policy.
 */

EpsilonGreedyBandit::EpsilonGreedyBandit(std::vector<PriceArm> arms,
                                         double epsilon,
                                         std::uint_fast32_t seed)
    : arms_(std::move(arms)),
      epsilon_(epsilon),
      rng_(seed),
      uniform01_(0.0, 1.0) {
    if (arms_.empty()) {
        throw std::invalid_argument("EpsilonGreedyBandit requires at least one arm");
    }
    if (epsilon_ < 0.0 || epsilon_ > 1.0) {
        throw std::invalid_argument("epsilon must lie within [0, 1]");
    }
}

std::size_t EpsilonGreedyBandit::selectArm(bool* wasExploration) {
    // Draw a uniform random number in [0, 1). If it is below epsilon we explore;
    // otherwise we exploit the current best estimate.
    const bool explore = uniform01_(rng_) < epsilon_;
    if (wasExploration != nullptr) {
        *wasExploration = explore;
    }

    if (explore) {
        // --- Exploration: pick an arm uniformly at random -----------------
        std::uniform_int_distribution<std::size_t> pick(0, arms_.size() - 1);
        return pick(rng_);
    }

    // --- Exploitation: pick the arm with the greatest reward estimate -----
    return bestArmIndex();
}

void EpsilonGreedyBandit::updateArm(std::size_t armIndex, double reward) {
    if (armIndex >= arms_.size()) {
        throw std::out_of_range("armIndex is out of range in updateArm");
    }

    PriceArm& arm = arms_[armIndex];

    // Incremental average update. Incrementing the count first means the step
    // size on the very first pull is 1/1 = 1, so Q jumps straight to the first
    // observed reward (a natural, unbiased starting point).
    arm.pullCount += 1;
    arm.totalReward += reward;
    arm.estimatedReward +=
        (reward - arm.estimatedReward) / static_cast<double>(arm.pullCount);
}

std::size_t EpsilonGreedyBandit::bestArmIndex() const {
    // Linear scan over k arms. Ties are broken toward the lowest index because
    // the comparison is strictly greater-than.
    std::size_t best = 0;
    for (std::size_t i = 1; i < arms_.size(); ++i) {
        if (arms_[i].estimatedReward > arms_[best].estimatedReward) {
            best = i;
        }
    }
    return best;
}
