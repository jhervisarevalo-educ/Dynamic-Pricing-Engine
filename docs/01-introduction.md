# 1. Introduction

Setting the right price is one of the most impactful and difficult decisions a
business makes. Price too high and customers walk away; price too low and profit
per sale evaporates. The "correct" price is rarely known in advance because it
depends on demand, competitor behaviour, inventory, and customer psychology, all
of which must be *learned from experience*.

This project implements a **Dynamic Pricing Engine** that learns the most
profitable selling price for a product by experimenting with several candidate
prices and observing the profit each one generates. It uses the **epsilon-Greedy
Multi-Armed Bandit (MAB)** algorithm, a classic reinforcement-learning technique
for balancing two competing needs:

- **Exploration** - trying prices we are unsure about to gather information.
- **Exploitation** - charging the price that currently looks most profitable.

The Multi-Armed Bandit metaphor comes from a gambler facing a row of slot
machines ("one-armed bandits"), each with an unknown payout. The gambler must
decide which arms to pull, and how often, to maximise winnings. In our setting
each *arm* is a candidate price, *pulling* an arm means offering that price to a
batch of customers, and the *payout* is the profit earned.

The program is a self-contained C++17 console application. It:

1. Asks the user for the base price, number of rounds, demand level, competitor
   price, and inventory level.
2. Generates several candidate prices around the base price (-10%, -5%, 0%, +5%,
   +10%).
3. Runs a round-by-round simulation in which the epsilon-Greedy policy chooses a
   price, a market model simulates customer purchases, and the resulting profit
   updates the estimate for that price.
4. Reports, per round and in a final summary, how often each price was used, its
   average reward, the total profit, the best-performing price, and the regret.

The emphasis of this project is on **algorithm design, correctness, and
complexity analysis**, not merely on building an application. The remaining
documents develop the problem formally, derive the mathematics, prove why the
method converges, and analyse its time and space complexity.
