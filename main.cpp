// ============================================================================
//  Sales Ranking System
//  A console-based C++ program for a retail store (marketing theme).
//
//  Course : Algorithms and Complexities
//  Purpose: Demonstrate Linear Search and Merge Sort on real-world data
//           (products ranked by how many units they have sold).
//
//  This program uses ONLY standard C++ libraries and is written to be
//  beginner-friendly and modular: one function per menu option.
// ============================================================================

#include <iostream>   // std::cin, std::cout
#include <vector>     // std::vector (our product container)
#include <string>     // std::string
#include <limits>     // std::numeric_limits (for clearing bad input)
#include <iomanip>    // std::setw, std::fixed, std::setprecision (table output)
#include <algorithm>  // std::transform (for case-insensitive name search)
#include <cctype>     // std::tolower
#include <fstream>    // std::ifstream, std::ofstream (file input/output)
#include <sstream>    // std::stringstream (splitting CSV lines)
#include <chrono>     // std::chrono (measuring algorithm run time)
#include <random>     // std::mt19937 (generating random test data)

#include "product.hpp"                     // the shared Product class
#include "sort-algo/sort_algorithms.hpp"   // multiple sorting algorithms
#include "search-algo/search_algorithms.hpp" // linear + binary search

// ============================================================================
//  STAGE 2: Input validation helpers
//  ---------------------------------------------------------------------------
//  These small helpers keep the rest of the program clean. They repeatedly
//  ask the user until valid input is entered, so the menu never crashes on
//  bad input (for example, letters typed where a number is expected).
// ============================================================================

// Clears the leftover/invalid characters from the input buffer.
void clearInputBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// Reads a whole integer. Loops until the user types a valid whole number.
int readInt(const std::string& prompt) {
    int value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            clearInputBuffer(); // remove the trailing newline
            return value;
        }
        std::cout << "  Invalid input. Please enter a whole number.\n";
        clearInputBuffer();
    }
}

// Reads a non-negative price. Loops until a valid, non-negative number is given.
double readPrice(const std::string& prompt) {
    double value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value && value >= 0.0) {
            clearInputBuffer();
            return value;
        }
        std::cout << "  Invalid input. Please enter a non-negative number.\n";
        clearInputBuffer();
    }
}

// Reads a full line of text (allows spaces in names/categories).
std::string readLine(const std::string& prompt) {
    std::string value;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, value);
        if (!value.empty()) {
            return value;
        }
        std::cout << "  Input cannot be empty. Please try again.\n";
    }
}

// Returns a lowercase copy of a string (used for case-insensitive searching).
std::string toLower(const std::string& text) {
    std::string result = text;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

// ============================================================================
//  Display helpers
//  ---------------------------------------------------------------------------
//  Small utilities to print products in a neat, aligned table.
// ============================================================================

// Calculates the Name and Category column widths from the actual data so long
// product names or categories are not truncated. Starts from sensible minimums.
void calculateColumnWidths(const std::vector<Product>& products,
                           int& nameWidth, int& categoryWidth) {
    nameWidth     = 22; // minimum width for the Name column
    categoryWidth = 16; // minimum width for the Category column
    for (const Product& p : products) {
        if (static_cast<int>(p.name.length()) > nameWidth) {
            nameWidth = static_cast<int>(p.name.length());
        }
        if (static_cast<int>(p.category.length()) > categoryWidth) {
            categoryWidth = static_cast<int>(p.category.length());
        }
    }
    nameWidth     += 2; // a little padding between columns
    categoryWidth += 2;
}

// Prints the table header row using the given column widths.
void printTableHeader(int nameWidth, int categoryWidth) {
    std::cout << "\n"
              << std::left
              << std::setw(6)          << "ID"
              << std::setw(nameWidth)     << "Name"
              << std::setw(categoryWidth) << "Category"
              << std::right
              << std::setw(10) << "Price"
              << std::setw(12) << "Units Sold"
              << "\n";
    int totalWidth = 6 + nameWidth + categoryWidth + 10 + 12;
    std::cout << std::string(totalWidth, '-') << "\n";
}

// Prints a single product as one aligned table row using the given widths.
void printProductRow(const Product& p, int nameWidth, int categoryWidth) {
    std::cout << std::left
              << std::setw(6)          << p.id
              << std::setw(nameWidth)     << p.name
              << std::setw(categoryWidth) << p.category
              << std::right
              << std::setw(10) << std::fixed << std::setprecision(2) << p.price
              << std::setw(12) << p.unitsSold
              << "\n";
}

// ============================================================================
//  STAGE 3: Add a Product and Display All Products
// ============================================================================

// Adds a new product to the vector.
// Time complexity: O(n) because we check that the ID is unique first;
// the push_back itself is O(1) amortized.
void addProduct(std::vector<Product>& products) {
    std::cout << "\n--- Add a Product ---\n";

    int id = readInt("Enter Product ID: ");

    // Make sure the ID is not already used (linear scan).
    for (const Product& p : products) {
        if (p.id == id) {
            std::cout << "  A product with ID " << id
                      << " already exists. Product not added.\n";
            return;
        }
    }

    std::string name     = readLine("Enter Product Name: ");
    std::string category = readLine("Enter Category: ");
    double      price    = readPrice("Enter Price: ");

    // Build the product and store it in the vector.
    products.push_back(Product(id, name, category, price));
    std::cout << "  Product \"" << name << "\" added successfully!\n";
}

// Displays every product currently stored.
// Time complexity: O(n) - we visit each product exactly once.
void displayAllProducts(const std::vector<Product>& products) {
    std::cout << "\n--- All Products ---";
    if (products.empty()) {
        std::cout << "\n  No products to display. Add some first.\n";
        return;
    }
    int nameWidth, categoryWidth;
    calculateColumnWidths(products, nameWidth, categoryWidth);
    printTableHeader(nameWidth, categoryWidth);
    for (const Product& p : products) {
        printProductRow(p, nameWidth, categoryWidth);
    }
    std::cout << "\nTotal products: " << products.size() << "\n";
}

// ============================================================================
//  STAGE 4: Record a Sale and Search (Linear / Binary)
//  ---------------------------------------------------------------------------
//  The search algorithms now live in search-algo/search_algorithms.hpp:
//    * Linear Search - O(n),     works on unsorted data.
//    * Binary Search - O(log n), requires the data sorted by ID first.
//  Record a Sale uses Linear Search because the list is not kept sorted.
// ============================================================================

// Records a sale by increasing the units sold of a chosen product.
// Time complexity: O(n) because we must first find the product (Linear Search).
void recordSale(std::vector<Product>& products) {
    std::cout << "\n--- Record a Sale ---\n";
    if (products.empty()) {
        std::cout << "  No products available. Add a product first.\n";
        return;
    }

    int id    = readInt("Enter the Product ID of the item sold: ");
    int index = searchalgo::linearSearchById(products, id);

    if (index == -1) {
        std::cout << "  No product found with ID " << id << ".\n";
        return;
    }

    int quantity = readInt("Enter quantity sold: ");
    if (quantity <= 0) {
        std::cout << "  Quantity must be greater than 0. No sale recorded.\n";
        return;
    }

    products[index].unitsSold += quantity;
    std::cout << "  Recorded " << quantity << " sale(s) for \""
              << products[index].name << "\". New total: "
              << products[index].unitsSold << " units.\n";
}

// Searches for products by ID (Linear or Binary) or by partial name (Linear).
void searchProduct(const std::vector<Product>& products) {
    std::cout << "\n--- Search Product ---\n";
    if (products.empty()) {
        std::cout << "  No products to search. Add some first.\n";
        return;
    }

    std::cout << "Search by:\n"
              << "  1. Product ID\n"
              << "  2. Product Name\n";
    int choice = readInt("Enter choice (1-2): ");

    if (choice == 1) {
        // Let the user pick which search ALGORITHM to use for the ID search.
        std::cout << "Search algorithm:\n"
                  << "  1. Linear Search (O(n), no preparation needed)\n"
                  << "  2. Binary Search (O(log n), needs data sorted by ID first)\n";
        int algo = readInt("Enter choice (1-2): ");
        int id   = readInt("Enter Product ID: ");

        const Product* found = nullptr;

        if (algo == 2) {
            // Binary search needs a copy sorted by ID ascending.
            std::vector<Product> sorted = searchalgo::sortByIdAscending(products);
            int index = searchalgo::binarySearchById(sorted, id);
            if (index != -1) {
                found = &sorted[index];
            }
            std::cout << "  (Used Binary Search)\n";
        } else {
            int index = searchalgo::linearSearchById(products, id);
            if (index != -1) {
                found = &products[index];
            }
            std::cout << "  (Used Linear Search)\n";
        }

        if (found == nullptr) {
            std::cout << "  No product found with ID " << id << ".\n";
        } else {
            int nameWidth, categoryWidth;
            calculateColumnWidths(products, nameWidth, categoryWidth);
            printTableHeader(nameWidth, categoryWidth);
            printProductRow(*found, nameWidth, categoryWidth);
        }
    } else if (choice == 2) {
        std::string term = toLower(readLine("Enter Product Name (or part of it): "));
        bool found = false;

        int nameWidth, categoryWidth;
        calculateColumnWidths(products, nameWidth, categoryWidth);

        // Linear scan: show every product whose name contains the search term.
        for (const Product& p : products) {
            if (toLower(p.name).find(term) != std::string::npos) {
                if (!found) {
                    printTableHeader(nameWidth, categoryWidth); // header once, before first match
                    found = true;
                }
                printProductRow(p, nameWidth, categoryWidth);
            }
        }
        if (!found) {
            std::cout << "  No product found matching \"" << term << "\".\n";
        }
    } else {
        std::cout << "  Invalid choice.\n";
    }
}

// ============================================================================
//  STAGE 5: Sorting with a choice of algorithm + timing
//  ---------------------------------------------------------------------------
//  The sorting algorithms live in sort-algo/sort_algorithms.hpp. Here we let
//  the user pick one, run it, and measure how long it took in milliseconds.
// ============================================================================

// Runs a sort function on the given vector and returns the elapsed time in ms.
// Uses a high-resolution clock so even fast sorts can be measured.
double timeSort(void (*sortFn)(std::vector<Product>&),
                std::vector<Product>& data) {
    auto start = std::chrono::high_resolution_clock::now();
    sortFn(data);
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}

// Menu option: let the user choose a sorting algorithm, sort in place, time it.
void sortByUnitsSold(std::vector<Product>& products) {
    std::cout << "\n--- Sort Products by Units Sold (Highest to Lowest) ---\n";
    if (products.empty()) {
        std::cout << "  No products to sort. Add some first.\n";
        return;
    }

    // Show the available algorithms from the registry.
    std::vector<sortalgo::SortAlgo> algos = sortalgo::allSortAlgorithms();
    std::cout << "Choose a sorting algorithm:\n";
    for (std::size_t i = 0; i < algos.size(); ++i) {
        std::cout << "  " << (i + 1) << ". " << algos[i].name
                  << "  [" << algos[i].bigO << "]\n";
    }
    int choice = readInt("Enter choice (1-" +
                         std::to_string(algos.size()) + "): ");

    if (choice < 1 || choice > static_cast<int>(algos.size())) {
        std::cout << "  Invalid choice. Returning to menu.\n";
        return;
    }

    const sortalgo::SortAlgo& chosen = algos[choice - 1];
    double ms = timeSort(chosen.run, products); // sorts `products` in place

    std::cout << "\n  Sorted using " << chosen.name
              << " (" << chosen.bigO << ") in "
              << std::fixed << std::setprecision(4) << ms << " ms.\n";
    displayAllProducts(products);
}

// ============================================================================
//  STAGE 6: Top 5 Best-Selling and Lowest-Selling products
//  ---------------------------------------------------------------------------
//  Both features rank the products using the sort library's Merge Sort
//  (O(n log n), stable), then print from either end of the sorted list.
//  Sorting a copy keeps the caller's original ordering unchanged.
// ============================================================================

// Displays the 5 products with the highest units sold.
// Time complexity: O(n log n) (dominated by the sort).
void displayTopSelling(const std::vector<Product>& products) {
    std::cout << "\n--- Top 5 Best-Selling Products (ranked via Merge Sort) ---";
    if (products.empty()) {
        std::cout << "\n  No products to rank. Add some first.\n";
        return;
    }

    // Work on a copy so the original list order is not disturbed.
    std::vector<Product> ranked = products;
    sortalgo::mergeSort(ranked);

    int nameWidth, categoryWidth;
    calculateColumnWidths(ranked, nameWidth, categoryWidth);
    int count = std::min(5, static_cast<int>(ranked.size()));
    printTableHeader(nameWidth, categoryWidth);
    for (int i = 0; i < count; ++i) {
        printProductRow(ranked[i], nameWidth, categoryWidth); // highest first
    }
}

// Displays the lowest-selling products (bottom 5).
// Time complexity: O(n log n) (dominated by the sort).
void displayLowestSelling(const std::vector<Product>& products) {
    std::cout << "\n--- Lowest-Selling Products (ranked via Merge Sort) ---";
    if (products.empty()) {
        std::cout << "\n  No products to rank. Add some first.\n";
        return;
    }

    std::vector<Product> ranked = products;
    sortalgo::mergeSort(ranked);

    int nameWidth, categoryWidth;
    calculateColumnWidths(ranked, nameWidth, categoryWidth);
    int total = static_cast<int>(ranked.size());
    int count = std::min(5, total);
    printTableHeader(nameWidth, categoryWidth);
    // The lowest sellers are at the END of the descending-sorted list.
    for (int i = total - count; i < total; ++i) {
        printProductRow(ranked[i], nameWidth, categoryWidth);
    }
}

// ============================================================================
//  STAGE 7 (extension): File input/output using CSV
//  ---------------------------------------------------------------------------
//  Data is stored in a plain-text CSV file, one product per line:
//      id,name,category,price,unitsSold
//  Example:
//      101,Wireless Mouse,Electronics,19.99,120
//
//  This lets product data come from a file instead of being hard-coded, and
//  allows keeping several data files (e.g. products_2024.csv, products_2025.csv).
//
//  Beginner-friendly simplification: we assume product names and categories do
//  NOT contain commas, because a comma is our field separator.
// ============================================================================

// Saves every product to the given CSV file (overwrites the file if it exists).
// Returns true on success, false if the file could not be opened for writing.
// Time complexity: O(n) - each product is written exactly once.
bool saveToFile(const std::vector<Product>& products, const std::string& filename) {
    std::ofstream out(filename);
    if (!out) {
        return false; // could not open the file for writing
    }

    // Write one product per line as comma-separated values.
    for (const Product& p : products) {
        out << p.id       << ','
            << p.name     << ','
            << p.category << ','
            << p.price    << ','
            << p.unitsSold << '\n';
    }
    return true;
}

// Loads products from the given CSV file, REPLACING the current list.
// Returns true on success, false if the file could not be opened (e.g. missing).
// Time complexity: O(n) - each line is read and parsed exactly once.
bool loadFromFile(std::vector<Product>& products, const std::string& filename) {
    std::ifstream in(filename);
    if (!in) {
        return false; // file does not exist or cannot be read
    }

    std::vector<Product> loaded; // build into a temporary list first
    std::string line;

    while (std::getline(in, line)) {
        if (line.empty()) {
            continue; // skip blank lines
        }

        // Split the line into fields separated by commas.
        std::stringstream ss(line);
        std::string idText, name, category, priceText, unitsText;

        if (std::getline(ss, idText,    ',') &&
            std::getline(ss, name,      ',') &&
            std::getline(ss, category,  ',') &&
            std::getline(ss, priceText, ',') &&
            std::getline(ss, unitsText)) {

            // Convert the text fields into numbers.
            // try/catch keeps one bad line from crashing the whole load.
            try {
                Product p(std::stoi(idText), name, category, std::stod(priceText));
                p.unitsSold = std::stoi(unitsText);
                loaded.push_back(p);
            } catch (const std::exception&) {
                std::cout << "  Skipping a malformed line: " << line << "\n";
            }
        } else {
            std::cout << "  Skipping a malformed line: " << line << "\n";
        }
    }

    products = loaded; // replace the current data with what we loaded
    return true;
}

// Menu option: ask for a filename and save the current products to it.
void saveProductsMenu(const std::vector<Product>& products,
                      const std::string& defaultFile) {
    std::cout << "\n--- Save Products to File ---\n";

    // Read a filename that MAY be empty (unlike readLine, which requires input).
    // If the user just presses Enter, we save to the default file.
    std::cout << "Enter filename to save to (press Enter for \""
              << defaultFile << "\"): ";
    std::string filename;
    std::getline(std::cin, filename);
    if (filename.empty()) {
        filename = defaultFile;
    }

    if (saveToFile(products, filename)) {
        std::cout << "  Saved " << products.size()
                  << " product(s) to \"" << filename << "\".\n";
    } else {
        std::cout << "  Error: could not open \"" << filename
                  << "\" for writing.\n";
    }
}

// ============================================================================
//  STAGE 8 (extension): Random data generator + algorithm benchmark
//  ---------------------------------------------------------------------------
//  With only a handful of products, every sort finishes in ~0 ms. To see the
//  difference between O(n^2) and O(n log n) algorithms we can generate a large
//  random dataset and then time each algorithm on the SAME data.
// ============================================================================

// Beyond this size the O(n^2) sorts become very slow, so we skip them.
const int QUADRATIC_LIMIT = 20000;

// Menu option: replace the current list with N randomly generated products.
// Time complexity: O(n).
void generateRandomProducts(std::vector<Product>& products) {
    std::cout << "\n--- Generate Random Products ---\n";
    int n = readInt("How many random products to generate? ");
    if (n <= 0) {
        std::cout << "  Please enter a positive number. Nothing generated.\n";
        return;
    }

    // A few sample categories to pick from at random.
    const std::vector<std::string> categories = {
        "Electronics", "Kitchen", "Stationery", "Sports", "Toys", "Clothing"
    };

    // Random number generation using the standard <random> library.
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> unitsDist(0, 100000); // random units sold
    std::uniform_int_distribution<int> priceCents(50, 50000); // 0.50 - 500.00
    std::uniform_int_distribution<int> catPick(0, static_cast<int>(categories.size()) - 1);

    std::vector<Product> generated;
    generated.reserve(n);
    for (int i = 1; i <= n; ++i) {
        // Sequential IDs guarantee uniqueness; names are "Product_<id>".
        Product p(i, "Product_" + std::to_string(i),
                  categories[catPick(rng)], priceCents(rng) / 100.0);
        p.unitsSold = unitsDist(rng);
        generated.push_back(p);
    }

    products = generated; // replace the current data
    std::cout << "  Generated " << products.size()
              << " random product(s). They are now the active list.\n";
}

// Menu option: run EVERY sorting algorithm on the same data and print timings.
// Each algorithm sorts its own fresh copy so the comparison is fair.
void compareAllSorts(const std::vector<Product>& products) {
    std::cout << "\n--- Compare All Sorting Algorithms ---\n";
    if (products.empty()) {
        std::cout << "  No products to sort. Add or generate some first.\n";
        return;
    }

    std::size_t n = products.size();
    std::cout << "Sorting " << n << " product(s) with each algorithm...\n";
    if (n > static_cast<std::size_t>(QUADRATIC_LIMIT)) {
        std::cout << "  (Note: O(n^2) sorts are skipped for n > "
                  << QUADRATIC_LIMIT << " because they would be too slow.)\n";
    }

    // Table header.
    std::cout << "\n" << std::left
              << std::setw(18) << "Algorithm"
              << std::setw(14) << "Complexity"
              << std::right << std::setw(16) << "Time (ms)" << "\n";
    std::cout << std::string(48, '-') << "\n";

    for (const sortalgo::SortAlgo& algo : sortalgo::allSortAlgorithms()) {
        // Skip quadratic sorts on very large inputs.
        if (algo.isQuadratic && n > static_cast<std::size_t>(QUADRATIC_LIMIT)) {
            std::cout << std::left
                      << std::setw(18) << algo.name
                      << std::setw(14) << algo.bigO
                      << std::right << std::setw(16) << "skipped" << "\n";
            continue;
        }

        std::vector<Product> copy = products; // fresh, identical data each time
        double ms = timeSort(algo.run, copy);
        std::cout << std::left
                  << std::setw(18) << algo.name
                  << std::setw(14) << algo.bigO
                  << std::right << std::setw(16)
                  << std::fixed << std::setprecision(4) << ms << "\n";
    }
    std::cout << "\n  Tip: notice how O(n log n) sorts scale far better than O(n^2).\n";
}

// ============================================================================
//  Menu and main program loop
// ============================================================================

// Prints the main menu options.
void printMenu() {
    std::cout << "\n========================================\n"
              << "        SALES RANKING SYSTEM\n"
              << "========================================\n"
              << "  1. Add a Product\n"
              << "  2. Display All Products\n"
              << "  3. Record a Sale\n"
              << "  4. Search Product by ID or Name\n"
              << "  5. Sort Products by Units Sold\n"
              << "  6. Display Top 5 Best-Selling Products\n"
              << "  7. Display Lowest-Selling Products\n"
              << "  8. Save Products to File\n"
              << "  9. Generate Random Products\n"
              << " 10. Compare All Sorting Algorithms\n"
              << " 11. Exit\n"
              << "========================================\n";
}

// Adds a few sample products so the program is easy to demonstrate/test.
void loadSampleData(std::vector<Product>& products) {
    products.push_back(Product(101, "Wireless Mouse",   "Electronics", 19.99));
    products.push_back(Product(102, "Coffee Mug",       "Kitchen",      8.50));
    products.push_back(Product(103, "Notebook",         "Stationery",   3.25));
    products.push_back(Product(104, "Bluetooth Speaker","Electronics", 45.00));
    products.push_back(Product(105, "Water Bottle",     "Sports",      12.75));

    // Give them some starting sales so rankings are meaningful.
    products[0].unitsSold = 120;
    products[1].unitsSold = 45;
    products[2].unitsSold = 200;
    products[3].unitsSold = 75;
    products[4].unitsSold = 30;
}

int main() {
    // The default data file that is auto-loaded on start and auto-saved on exit.
    const std::string DEFAULT_FILE = "products.csv";

    std::vector<Product> products; // the main container for all products

    std::cout << "Welcome to the Sales Ranking System!\n";

    // Try to load saved data from the default file. If the file does not exist
    // (for example, the very first run), fall back to the built-in sample data
    // so the program is never empty for a demonstration.
    if (loadFromFile(products, DEFAULT_FILE) && !products.empty()) {
        std::cout << "Loaded " << products.size()
                  << " product(s) from \"" << DEFAULT_FILE << "\".\n";
    } else {
        loadSampleData(products);
        std::cout << "No saved data found. Sample products have been pre-loaded.\n";
    }

    bool running = true;
    while (running) {
        printMenu();
        int choice = readInt("Enter your choice (1-11): ");

        switch (choice) {
            case 1:  addProduct(products);                    break;
            case 2:  displayAllProducts(products);            break;
            case 3:  recordSale(products);                    break;
            case 4:  searchProduct(products);                 break;
            case 5:  sortByUnitsSold(products);               break;
            case 6:  displayTopSelling(products);             break;
            case 7:  displayLowestSelling(products);          break;
            case 8:  saveProductsMenu(products, DEFAULT_FILE);break;
            case 9:  generateRandomProducts(products);        break;
            case 10: compareAllSorts(products);               break;
            case 11:
                // Auto-save the current data so it is there next time.
                if (saveToFile(products, DEFAULT_FILE)) {
                    std::cout << "\nData saved to \"" << DEFAULT_FILE << "\".\n";
                } else {
                    std::cout << "\nWarning: could not save data to \""
                              << DEFAULT_FILE << "\".\n";
                }
                std::cout << "Thank you for using the Sales Ranking System. Goodbye!\n";
                running = false;
                break;
            default:
                std::cout << "  Invalid choice. Please enter a number from 1 to 11.\n";
                break;
        }
    }

    return 0;
}
