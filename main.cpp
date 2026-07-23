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

// ============================================================================
//  STAGE 1: The data model
//  ---------------------------------------------------------------------------
//  A Product is one item sold by the store. We store every product inside a
//  std::vector<Product>, which grows automatically as products are added.
// ============================================================================
class Product {
public:
    int         id;         // Unique product ID
    std::string name;       // Product name (e.g. "Wireless Mouse")
    std::string category;   // Product category (e.g. "Electronics")
    double      price;      // Selling price
    int         unitsSold;  // Total units sold so far (used for ranking)

    // Constructor: create a product with a starting sales count of 0.
    Product(int id, const std::string& name, const std::string& category,
            double price)
        : id(id), name(name), category(category), price(price), unitsSold(0) {}
};

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

// Prints the table header row.
void printTableHeader() {
    std::cout << "\n"
              << std::left
              << std::setw(6)  << "ID"
              << std::setw(22) << "Name"
              << std::setw(16) << "Category"
              << std::right
              << std::setw(10) << "Price"
              << std::setw(12) << "Units Sold"
              << "\n";
    std::cout << std::string(66, '-') << "\n";
}

// Prints a single product as one aligned table row.
void printProductRow(const Product& p) {
    std::cout << std::left
              << std::setw(6)  << p.id
              << std::setw(22) << p.name
              << std::setw(16) << p.category
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
    printTableHeader();
    for (const Product& p : products) {
        printProductRow(p);
    }
    std::cout << "\nTotal products: " << products.size() << "\n";
}

// ============================================================================
//  STAGE 4: Record a Sale and Linear Search
//  ---------------------------------------------------------------------------
//  LINEAR SEARCH is chosen for searching because:
//    * The product list is NOT kept sorted by ID or name, so faster methods
//      like binary search cannot be used directly.
//    * It is simple, correct, and works on any data ordering.
//    * Time complexity is O(n): in the worst case we look at every product.
// ============================================================================

// Returns the index of the product with the given ID, or -1 if not found.
// Time complexity: O(n) (Linear Search).
int linearSearchById(const std::vector<Product>& products, int id) {
    for (std::size_t i = 0; i < products.size(); ++i) {
        if (products[i].id == id) {
            return static_cast<int>(i);
        }
    }
    return -1; // not found
}

// Records a sale by increasing the units sold of a chosen product.
// Time complexity: O(n) because we must first find the product.
void recordSale(std::vector<Product>& products) {
    std::cout << "\n--- Record a Sale ---\n";
    if (products.empty()) {
        std::cout << "  No products available. Add a product first.\n";
        return;
    }

    int id    = readInt("Enter the Product ID of the item sold: ");
    int index = linearSearchById(products, id);

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

// Searches for products by ID or by (partial, case-insensitive) name.
// Time complexity: O(n) (Linear Search).
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
        int id    = readInt("Enter Product ID: ");
        int index = linearSearchById(products, id);
        if (index == -1) {
            std::cout << "  No product found with ID " << id << ".\n";
        } else {
            printTableHeader();
            printProductRow(products[index]);
        }
    } else if (choice == 2) {
        std::string term = toLower(readLine("Enter Product Name (or part of it): "));
        bool found = false;

        // Linear scan: show every product whose name contains the search term.
        for (const Product& p : products) {
            if (toLower(p.name).find(term) != std::string::npos) {
                if (!found) {
                    printTableHeader(); // print header once, before first match
                    found = true;
                }
                printProductRow(p);
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
//  STAGE 5: Merge Sort ranking (units sold, high to low)
//  ---------------------------------------------------------------------------
//  MERGE SORT is chosen for ranking because:
//    * It is a stable sort (products with equal sales keep their order).
//    * It guarantees O(n log n) time in the BEST, AVERAGE, and WORST cases,
//      unlike Quick Sort which can degrade to O(n^2) in the worst case.
//    * It is a classic divide-and-conquer algorithm, ideal for coursework.
//  Trade-off: it uses O(n) extra memory for the temporary merge buffers.
// ============================================================================

// Merges two already-sorted halves [left..mid] and [mid+1..right] so that
// the combined range is sorted in DESCENDING order of unitsSold.
void merge(std::vector<Product>& products, int left, int mid, int right) {
    // Copy the two halves into temporary vectors.
    std::vector<Product> leftHalf(products.begin() + left,
                                   products.begin() + mid + 1);
    std::vector<Product> rightHalf(products.begin() + mid + 1,
                                    products.begin() + right + 1);

    std::size_t i = 0; // index into leftHalf
    std::size_t j = 0; // index into rightHalf
    int k = left;      // index into the original vector

    // Pick the larger unitsSold first (descending order).
    while (i < leftHalf.size() && j < rightHalf.size()) {
        if (leftHalf[i].unitsSold >= rightHalf[j].unitsSold) {
            products[k++] = leftHalf[i++];
        } else {
            products[k++] = rightHalf[j++];
        }
    }

    // Copy any remaining elements from the left half.
    while (i < leftHalf.size()) {
        products[k++] = leftHalf[i++];
    }
    // Copy any remaining elements from the right half.
    while (j < rightHalf.size()) {
        products[k++] = rightHalf[j++];
    }
}

// Recursively splits the range in half, sorts each half, then merges them.
// Time complexity: O(n log n).
void mergeSort(std::vector<Product>& products, int left, int right) {
    if (left >= right) {
        return; // a range of 0 or 1 element is already sorted
    }
    int mid = left + (right - left) / 2; // avoids potential overflow
    mergeSort(products, left, mid);       // sort left half
    mergeSort(products, mid + 1, right);  // sort right half
    merge(products, left, mid, right);    // combine the two sorted halves
}

// Menu option: sort the products in place (highest units sold first) and show.
// Time complexity: O(n log n).
void sortByUnitsSold(std::vector<Product>& products) {
    std::cout << "\n--- Sort Products by Units Sold (Highest to Lowest) ---";
    if (products.empty()) {
        std::cout << "\n  No products to sort. Add some first.\n";
        return;
    }
    mergeSort(products, 0, static_cast<int>(products.size()) - 1);
    std::cout << "\n  Products sorted using Merge Sort.\n";
    displayAllProducts(products);
}

// ============================================================================
//  STAGE 6: Top 5 Best-Selling and Lowest-Selling products
//  ---------------------------------------------------------------------------
//  Both features first rank the products with Merge Sort (O(n log n)), then
//  print from either end of the sorted list. Sorting a copy keeps the caller's
//  original ordering unchanged.
// ============================================================================

// Displays the 5 products with the highest units sold.
// Time complexity: O(n log n) (dominated by the sort).
void displayTopSelling(const std::vector<Product>& products) {
    std::cout << "\n--- Top 5 Best-Selling Products ---";
    if (products.empty()) {
        std::cout << "\n  No products to rank. Add some first.\n";
        return;
    }

    // Work on a copy so the original list order is not disturbed.
    std::vector<Product> ranked = products;
    mergeSort(ranked, 0, static_cast<int>(ranked.size()) - 1);

    int count = std::min(5, static_cast<int>(ranked.size()));
    printTableHeader();
    for (int i = 0; i < count; ++i) {
        printProductRow(ranked[i]); // highest sales are at the front
    }
}

// Displays the lowest-selling products (bottom 5).
// Time complexity: O(n log n) (dominated by the sort).
void displayLowestSelling(const std::vector<Product>& products) {
    std::cout << "\n--- Lowest-Selling Products ---";
    if (products.empty()) {
        std::cout << "\n  No products to rank. Add some first.\n";
        return;
    }

    std::vector<Product> ranked = products;
    mergeSort(ranked, 0, static_cast<int>(ranked.size()) - 1);

    int total = static_cast<int>(ranked.size());
    int count = std::min(5, total);
    printTableHeader();
    // The lowest sellers are at the END of the descending-sorted list.
    for (int i = total - count; i < total; ++i) {
        printProductRow(ranked[i]);
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
void saveProductsMenu(const std::vector<Product>& products) {
    std::cout << "\n--- Save Products to File ---\n";
    std::string filename = readLine("Enter filename to save to (e.g. products.csv): ");
    if (saveToFile(products, filename)) {
        std::cout << "  Saved " << products.size()
                  << " product(s) to \"" << filename << "\".\n";
    } else {
        std::cout << "  Error: could not open \"" << filename
                  << "\" for writing.\n";
    }
}

// Menu option: ask for a filename and load products from it (replaces list).
void loadProductsMenu(std::vector<Product>& products) {
    std::cout << "\n--- Load Products from File ---\n";
    std::string filename = readLine("Enter filename to load from (e.g. products.csv): ");
    if (loadFromFile(products, filename)) {
        std::cout << "  Loaded " << products.size()
                  << " product(s) from \"" << filename << "\".\n";
    } else {
        std::cout << "  Error: could not open \"" << filename
                  << "\". Make sure the file exists.\n";
    }
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
              << "  9. Load Products from File\n"
              << " 10. Exit\n"
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
        int choice = readInt("Enter your choice (1-10): ");

        switch (choice) {
            case 1: addProduct(products);          break;
            case 2: displayAllProducts(products);  break;
            case 3: recordSale(products);          break;
            case 4: searchProduct(products);       break;
            case 5: sortByUnitsSold(products);     break;
            case 6: displayTopSelling(products);   break;
            case 7: displayLowestSelling(products);break;
            case 8: saveProductsMenu(products);    break;
            case 9: loadProductsMenu(products);    break;
            case 10:
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
                std::cout << "  Invalid choice. Please enter a number from 1 to 10.\n";
                break;
        }
    }

    return 0;
}
