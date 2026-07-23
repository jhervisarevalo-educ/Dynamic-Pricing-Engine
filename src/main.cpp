/**
 * @file main.cpp
 * @brief Console entry point for the Dynamic Pricing Engine.
 *
 * Handles user interaction: collects and validates the simulation parameters,
 * runs the epsilon-Greedy Multi-Armed Bandit pricing simulation, and prints the
 * per-round learning progress followed by the final report.
 *
 * The program has no dependencies beyond the C++17 standard library.
 */

#include <cctype>
#include <iostream>
#include <sstream>
#include <string>

#include "PricingEngine.hpp"
#include "SimulationConfig.hpp"

namespace {

/**
 * @brief Prompt for and read a validated numeric value from standard input.
 *
 * Repeats the prompt until the user enters a value that parses correctly and
 * satisfies the [minValue, maxValue] bounds. Returns @p defaultValue if the
 * user simply presses Enter on an empty line.
 *
 * @tparam T        Numeric type (e.g. double or std::size_t).
 * @param prompt    Text shown to the user.
 * @param minValue  Inclusive lower bound.
 * @param maxValue  Inclusive upper bound.
 * @param defaultValue Value used when the input line is empty.
 */
template <typename T>
T readValue(const std::string& prompt, T minValue, T maxValue, T defaultValue) {
    while (true) {
        std::cout << prompt << " [default " << defaultValue << "]: ";

        std::string line;
        if (!std::getline(std::cin, line)) {
            // End-of-input (e.g. piped input exhausted): fall back to default.
            std::cout << defaultValue << "\n";
            return defaultValue;
        }

        // Empty line -> accept the default.
        bool onlySpaces = true;
        for (char ch : line) {
            if (!std::isspace(static_cast<unsigned char>(ch))) {
                onlySpaces = false;
                break;
            }
        }
        if (onlySpaces) {
            return defaultValue;
        }

        // Attempt to parse the value.
        std::istringstream stream(line);
        T value{};
        if (!(stream >> value)) {
            std::cout << "  Invalid number, please try again.\n";
            continue;
        }

        if (value < minValue || value > maxValue) {
            std::cout << "  Value must be between " << minValue << " and "
                      << maxValue << ".\n";
            continue;
        }
        return value;
    }
}

}  // namespace

int main() {
    std::cout << "============================================================\n";
    std::cout << "   Dynamic Pricing Engine (epsilon-Greedy MAB)\n";
    std::cout << "============================================================\n";
    std::cout << "Press Enter at any prompt to accept the shown default.\n\n";

    SimulationConfig config;

    // Collect and validate all inputs required by the project brief.
    config.basePrice =
        readValue<double>("Base price", 0.01, 1'000'000.0, 100.0);
    config.numRounds =
        readValue<std::size_t>("Number of pricing rounds", 1, 10'000'000, 1000);
    config.demandLevel =
        readValue<double>("Demand level (0-1)", 0.0, 1.0, 0.6);
    config.competitorPrice =
        readValue<double>("Competitor price", 0.01, 1'000'000.0, 100.0);
    config.inventoryLevel = readValue<std::size_t>(
        "Inventory level (total units)", 1, 1'000'000'000, 100'000);

    // Algorithm / model parameters.
    config.epsilon =
        readValue<double>("Exploration rate epsilon (0-1)", 0.0, 1.0, 0.1);
    config.customersPerRound =
        readValue<std::size_t>("Customers per round", 1, 1'000'000, 100);
    config.costFraction = readValue<double>(
        "Unit cost as fraction of base price (0-1)", 0.0, 0.99, 0.6);

    // Verbosity: verbose prints every round (useful for small runs).
    const std::size_t verboseChoice = readValue<std::size_t>(
        "Verbose per-round output? (1 = yes, 0 = no)", 0, 1,
        config.numRounds <= 50 ? 1 : 0);
    const bool verbose = (verboseChoice == 1);

    std::cout << "\nDerived unit cost: " << config.unitCost()
              << " (profit per unit = price - unit cost)\n";

    // Run the simulation.
    PricingEngine engine(config);

    std::cout << "\nCandidate prices generated from the base price:\n  ";
    for (double price : engine.candidatePrices()) {
        std::cout << price << "  ";
    }
    std::cout << "\n";

    const PricingEngine::Report report = engine.run(std::cout, verbose);
    engine.printFinalReport(std::cout, report);

    return 0;
}
