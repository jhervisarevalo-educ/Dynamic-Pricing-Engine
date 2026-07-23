C++ Algorithms and Complexities Project Prompt

You are an expert C++ software engineer, algorithms professor, and technical writer.

I need you to design and implement a complete university-level project in C++ for my Algorithms and Complexities course.

Project Title

Dynamic Pricing Engine using an ε-Greedy Multi-Armed Bandit Algorithm

Project Objective

Develop a pricing engine that recommends the best selling price for a product by learning from previous sales. The algorithm should balance exploration and exploitation using the ε-Greedy Multi-Armed Bandit algorithm.

The project should emphasize algorithm design, complexity analysis, and correctness, not just creating an application.

Functional Requirements

The program should:

Run in the console.

Allow the user to enter: 

Base price

Number of pricing rounds (simulated customers)

Demand level

Competitor price

Inventory level

Generate several possible selling prices (for example: -10%, -5%, base price, +5%, +10%).

Use an ε-Greedy Multi-Armed Bandit algorithm to choose which price to test.

Simulate customer purchases and calculate the resulting profit.

Update the estimated reward for each price after every round.

Display the chosen price, reward, and updated statistics after each round.

At the end of the simulation, display: 

Number of times each price was selected

Average reward for each price

Total profit

Best-performing price

Regret (if implemented)

Algorithm Requirements

Implement the ε-Greedy Multi-Armed Bandit algorithm from scratch.

Do NOT use machine learning libraries or external AI libraries.

The implementation should clearly show:

Exploration

Exploitation

Reward updating

Action selection

Incremental average calculation

Explain why the algorithm converges toward the optimal price over time.

C++ Requirements

Use:

Classes

Structs

Vectors

Functions

Random number generation using <random>

Proper object-oriented design

Clean modular code

Meaningful comments

Avoid global variables whenever possible.

Documentation Required

Provide the following sections:

Introduction

Problem Statement

Objectives

Algorithm Explanation

Mathematical Model

Pseudocode

Flowchart (ASCII or Mermaid)

Time Complexity Analysis

Space Complexity Analysis

Correctness Explanation

Advantages

Limitations

Possible Future Improvements

Complexity Analysis

Analyze:

Best Case

Average Case

Worst Case

Discuss:

O(n)

O(k)

O(nk)

Explain why k is considered constant when the number of possible prices is fixed.

Output Requirements

Provide sample executions using realistic data.

Show several rounds of learning and demonstrate how the algorithm eventually prefers the most profitable price.

Code Quality

The code should be:

Modular

Readable

Well documented

Production-quality

Suitable for presentation to a university professor

Deliverables

Provide:

Complete C++ source code

Detailed explanation of every function

Complete algorithm walkthrough

Sample inputs and outputs

Time and space complexity analysis

Explanation of why ε-Greedy is appropriate for dynamic pricing

Suggestions for additional features if more time is available

Assume this project will be graded heavily on algorithm design, correctness, complexity analysis, documentation, and code quality. Prioritize clear explanations and proper software engineering practices over brevity.
