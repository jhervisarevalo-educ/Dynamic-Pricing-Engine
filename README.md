# Sales Ranking System

A console-based C++ program for a retail store, built for the **Algorithms and Complexities** course. It demonstrates two classic algorithms, **Linear Search** and **Merge Sort**, applied to a real-world marketing task: ranking products by how many units they have sold.

---

## 1. Project Overview

Retail stores need to know which products are selling well and which are not, so they can make marketing and stocking decisions. This program lets a user manage a list of products, record sales, search for products, and rank them by total units sold.

The program is menu-driven and runs entirely in the console (no GUI). It uses object-oriented programming and only the C++ standard library.

---

## 2. Objectives

- Practice object-oriented programming in C++ (a `Product` class stored in a `std::vector`).
- Implement and explain **Linear Search** for finding products.
- Implement and explain **Merge Sort** for ranking products by units sold.
- Analyze the **time complexity** (Big-O) of every major operation.
- Write clean, modular, well-commented, beginner-friendly code with input validation.

---

## 3. Features (Menu Options)

1. **Add a Product** - enter ID, name, category, and price.
2. **Display All Products** - show every product in a table.
3. **Record a Sale** - increase the units sold of a product.
4. **Search Product by ID or Name** - find products quickly.
5. **Sort Products by Units Sold** - rank highest to lowest (Merge Sort).
6. **Display Top 5 Best-Selling Products**.
7. **Display Lowest-Selling Products**.
8. **Save Products to File** - type a filename to save the current list (Save As). Supports keeping several data files.
9. **Load Products from File** - type a filename to load a saved list (replaces the current data).
10. **Exit** - auto-saves the current data to `products.csv`.

---

## 4. Data Model

Each product is an object of the `Product` class:

| Field       | Type          | Meaning                        |
|-------------|---------------|--------------------------------|
| `id`        | `int`         | Unique product ID              |
| `name`      | `std::string` | Product name                   |
| `category`  | `std::string` | Product category               |
| `price`     | `double`      | Selling price                  |
| `unitsSold` | `int`         | Total units sold (for ranking) |

All products are stored in a `std::vector<Product>`, which grows automatically as products are added.

---

## 4a. File Storage (CSV Input/Output)

Instead of relying only on hard-coded data, the program reads and writes product data using **CSV** (comma-separated values) files.

**Format** - one product per line, fields in this order:

```
id,name,category,price,unitsSold
```

**Example `products.csv`:**

```
101,Wireless Mouse,Electronics,19.99,120
102,Coffee Mug,Kitchen,8.5,45
103,Notebook,Stationery,3.25,200
104,Bluetooth Speaker,Electronics,45,75
105,Water Bottle,Sports,12.75,30
```

**How loading/saving works:**

- **On startup**, the program tries to auto-load the default file `products.csv`. If it exists, that data is used; if not, built-in sample data is loaded so the program is never empty.
- **On exit** (option 10), the current data is auto-saved back to `products.csv`.
- **Save Products to File** (option 8) and **Load Products from File** (option 9) let you type any filename, so you can keep **multiple data files** (e.g. `products_2024.csv`, `products_2025.csv`).

**Limitation (beginner-friendly simplification):** because a comma separates the fields, product names and categories should **not contain commas**. Any malformed line is skipped with a warning instead of crashing the program.

---

## 5. Algorithms Used

### Linear Search (for searching)
Linear Search checks each product one by one until it finds a match.

**Why chosen:**
- The product list is **not kept sorted** by ID or name, so a faster method like binary search cannot be used directly.
- It is **simple and always correct**, regardless of how the data is ordered.
- It works for both exact ID matches and partial name matches.

**Time complexity:** `O(n)` - in the worst case, every product is checked.

### Merge Sort (for ranking)
Merge Sort is a divide-and-conquer algorithm: it splits the list in half, sorts each half recursively, then merges the two sorted halves back together (in descending order of units sold).

**Why chosen:**
- It **guarantees `O(n log n)`** in the best, average, and worst cases, unlike Quick Sort, which can drop to `O(n^2)` in the worst case.
- It is **stable**: products with equal sales keep their original relative order.
- It is a clear, classic teaching example of divide-and-conquer.

**Time complexity:** `O(n log n)` time, `O(n)` extra space (for the temporary merge buffers).

---

## 6. Time Complexity Analysis (Big-O)

Let `n` be the number of products.

| Operation                     | Time Complexity | Notes                                           |
|-------------------------------|-----------------|-------------------------------------------------|
| Add a Product                 | `O(n)`          | Scans to reject duplicate IDs; `push_back` is `O(1)` amortized |
| Display All Products          | `O(n)`          | Visits each product once                        |
| Record a Sale                 | `O(n)`          | Linear Search to find the product first         |
| Search by ID                  | `O(n)`          | Linear Search                                   |
| Search by Name                | `O(n)`          | Linear Search over all products                 |
| Sort by Units Sold            | `O(n log n)`    | Merge Sort                                       |
| Top 5 Best-Selling            | `O(n log n)`    | Sorts a copy, then reads the top 5              |
| Lowest-Selling                | `O(n log n)`    | Sorts a copy, then reads the bottom 5           |
| Save to File                  | `O(n)`          | Writes each product as one CSV line             |
| Load from File                | `O(n)`          | Reads and parses each CSV line once             |

---

## 7. How to Build and Run

Requires a C++ compiler that supports C++17 (for example, `g++`).

```bash
# Compile
g++ -std=c++17 -Wall -Wextra -o sales_ranking main.cpp

# Run
./sales_ranking
```

On the first run there is no `products.csv`, so the program pre-loads a few sample products. When you exit, your data is saved to `products.csv` and will be auto-loaded next time.

---

## 8. Sample Input and Output

Below is a short sample session (using the pre-loaded sample products).

**Displaying all products (menu option 2):**

```
--- All Products ---
ID    Name                  Category             Price  Units Sold
------------------------------------------------------------------
101   Wireless Mouse        Electronics          19.99         120
102   Coffee Mug            Kitchen               8.50          45
103   Notebook              Stationery            3.25         200
104   Bluetooth Speaker     Electronics          45.00          75
105   Water Bottle          Sports               12.75          30

Total products: 5
```

**Top 5 Best-Selling (menu option 6) - ranked with Merge Sort:**

```
--- Top 5 Best-Selling Products ---
ID    Name                  Category             Price  Units Sold
------------------------------------------------------------------
103   Notebook              Stationery            3.25         200
101   Wireless Mouse        Electronics          19.99         120
104   Bluetooth Speaker     Electronics          45.00          75
102   Coffee Mug            Kitchen               8.50          45
105   Water Bottle          Sports               12.75          30
```

**Recording a sale (menu option 3):**

```
--- Record a Sale ---
Enter the Product ID of the item sold: 103
Enter quantity sold: 50
  Recorded 50 sale(s) for "Notebook". New total: 250 units.
```

**Searching by name (menu option 4, then 2):**

```
--- Search Product ---
Search by:
  1. Product ID
  2. Product Name
Enter choice (1-2): 2
Enter Product Name (or part of it): mouse
ID    Name                  Category             Price  Units Sold
------------------------------------------------------------------
101   Wireless Mouse        Electronics          19.99         120
```

**Saving to a named file (menu option 8):**

```
--- Save Products to File ---
Enter filename to save to (e.g. products.csv): backup_2026.csv
  Saved 5 product(s) to "backup_2026.csv".
```

**Auto-load on the next run and auto-save on exit (option 10):**

```
Welcome to the Sales Ranking System!
Loaded 5 product(s) from "products.csv".
...
Enter your choice (1-10): 10

Data saved to "products.csv".
Thank you for using the Sales Ranking System. Goodbye!
```

---

## 9. Possible Future Improvements

- **Binary Search** on an ID-sorted copy for faster lookups on very large datasets.
- **Quoted CSV fields** so names and categories can safely contain commas.
- **Sort by other fields** (price, category, name), ascending or descending.
- **Sales reports** such as total revenue per category (`price * unitsSold`).
- **Edit or delete products** from the list.
- **Undo a sale** in case of mistakes.

---

## 10. File Structure

| File           | Purpose                                                   |
|----------------|-----------------------------------------------------------|
| `main.cpp`     | Full program: `Product` class, menu, algorithms, file I/O |
| `README.md`    | This documentation                                        |
| `products.csv` | Default data file (auto-created on first exit)             |
