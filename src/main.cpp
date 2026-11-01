#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <chrono>
#include <random>
#include "Airport.h"
#include "AirportBST.h"

// Split one line into fields. A comma ends a field unless we are inside double
// quotes. Inside quotes, two quote marks in a row ("") mean one literal quote
std::vector<std::string> splitFields(const std::string& line) {
    std::vector<std::string> fields;
    std::string field;
    bool inQuotes = false;
    int n = (int)line.size();
    for (int i = 0; i < n; i++) {
        char c = line[i];
        if (inQuotes) {
            if (c == '"') {
                if (i + 1 < n && line[i + 1] == '"') {
                    field += '"';
                    i++;
                } else {
                    inQuotes = false;
                }
            } else {
                field += c;
            }
        } else if (c == '"') {
            inQuotes = true;
        } else if (c == ',') {
            fields.push_back(field);
            field.clear();
        } else if (c != '\r') {
            field += c;
        }
    }
    fields.push_back(field);
    return fields;
}

// True if the text is one or more digits (used to recognise a header line)
bool allDigits(const std::string& s) {
    int n = (int)s.size();
    if (n == 0) return false;
    for (int i = 0; i < n; i++) {
        if (s[i] < '0' || s[i] > '9') return false;
    }
    return true;
}

// Read the data file into 'airports'. A line is skipped when its IATA code is
// not exactly 3 characters. The header line is ignored and not counted.
// Returns false if the file won't open.
bool loadAirports(const std::string& path, std::vector<Airport>& airports, int& skipped) {
    std::ifstream in(path.c_str());
    if (!in) return false;
    airports.clear();
    skipped = 0;
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty() || line == "\r") continue;
        std::vector<std::string> f = splitFields(line);
        if (f.size() < 8 || !allDigits(f[0])) continue;
        if (f[4].size() != 3) {
            skipped++;
            continue;
        }
        Airport a;
        a.code = f[4];
        a.name = f[1];
        a.city = f[2];
        a.country = f[3];
        a.icao = f[5];
        a.latitude = std::strtod(f[6].c_str(), nullptr);
        a.longitude = std::strtod(f[7].c_str(), nullptr);
        airports.push_back(a);
    }
    return true;
}

// Prints the tree's height and average depth.
void printShape(const AirportBST& tree) {
    std::cout << "Height: " << tree.height()
              << "   Average depth: " << std::fixed << std::setprecision(1)
              << tree.averageDepth() << "\n";
}

// Prints the top three levels of the tree, one line per level ("-" = empty spot).
void printLevels(const AirportBST& tree) {
    std::vector<std::vector<std::string> > levels = tree.levelOrder(3);
    int n = (int)levels.size();
    for (int i = 0; i < n; i++) {
        std::cout << "  L" << i << ":";
        int m = (int)levels[i].size();
        for (int j = 0; j < m; j++) std::cout << " " << levels[i][j];
        std::cout << "\n";
    }
}

// Prints "validate: OK" or "validate: FAILED".
void printValidate(const AirportBST& tree) {
    std::cout << "validate: " << (tree.validate() ? "OK" : "FAILED") << "\n";
}

// Prints one airport as: CODE name, city, country.
void printAirport(const Airport& a) {
    std::cout << a.code << " " << a.name << ", " << a.city << ", " << a.country << "\n";
}

// Handles "find CODE": prints the search path, the comparison count and the airport.
void doFind(const AirportBST& tree, const std::string& code) {
    Airport a;
    std::vector<std::string> path;
    bool found = tree.find(code, a, path);
    int n = (int)path.size();
    std::cout << "Path: ";
    for (int i = 0; i < n; i++) {
        if (i > 0) std::cout << " -> ";
        std::cout << path[i];
    }
    if (found) {
        std::cout << "    (comparisons: " << n << ")\n";
        printAirport(a);
    } else {
        std::cout << (n > 0 ? " -> " : "") << "(NULL)    not found (comparisons: " << n << ")\n";
    }
}

// Handles "remove CODE": shows the top three levels before and after, then validates.
void doRemove(AirportBST& tree, const std::string& code) {
    std::cout << "Before:\n";
    printLevels(tree);
    std::string info;
    if (!tree.remove(code, info)) {
        std::cout << "Not found: " << code << "\n";
        return;
    }
    std::cout << "After:\n";
    printLevels(tree);
    std::cout << "Deleted " << code << " (case " << info << ")   ";
    printValidate(tree);
}

// Handles "add CODE name|city|country": inserts a new airport, then validates.
void doAdd(AirportBST& tree, const std::string& code, const std::string& rest) {
    std::vector<std::string> parts;
    std::string part;
    std::istringstream in(rest);
    while (std::getline(in, part, '|')) parts.push_back(part);
    if (code.size() != 3 || parts.size() != 3) {
        std::cout << "Usage: add CODE name|city|country   (code must be 3 letters)\n";
        return;
    }
    Airport a;
    a.code = code;
    a.name = parts[0];
    a.city = parts[1];
    a.country = parts[2];
    if (tree.insert(a)) {
        std::cout << "Added " << code << "   ";
        printValidate(tree);
    } else {
        std::cout << "Duplicate code: " << code << " (not added)\n";
    }
}

// Handles "list [N]": prints airports in alphabetical code order (all, or the first N)
// and reports whether the whole inorder list is sorted.
void doList(const AirportBST& tree, int limit) {
    std::vector<Airport> all = tree.inorderAirports();
    int n = (int)all.size();
    bool sorted = true;
    for (int i = 1; i < n; i++) {
        if (!(all[i - 1].code < all[i].code)) sorted = false;
    }
    int shown = (limit >= 0 && limit < n) ? limit : n;
    for (int i = 0; i < shown; i++) printAirport(all[i]);
    std::cout << shown << " of " << n << " airports shown, sorted: " << (sorted ? "YES" : "NO") << "\n";
}

// Handles "save FILE": writes the codes in preorder, one per line.
void doSave(const AirportBST& tree, const std::string& path) {
    std::ofstream out(path.c_str());
    if (!out) {
        std::cout << "Cannot write file: " << path << "\n";
        return;
    }
    std::vector<std::string> codes = tree.preorderCodes();
    int n = (int)codes.size();
    for (int i = 0; i < n; i++) out << codes[i] << "\n";
    std::cout << "Saved " << n << " codes (preorder) to " << path << "\n";
}

// Handles "loadtree FILE": rebuilds the tree by inserting the saved codes in file order,
// then compares its height and level order with the tree that was there before.
void doLoadTree(AirportBST& tree, const std::vector<Airport>& airports, const std::string& path) {
    std::ifstream in(path.c_str());
    if (!in) {
        std::cout << "Cannot open file: " << path << "\n";
        return;
    }
    std::vector<std::string> codes;
    std::string code;
    while (in >> code) codes.push_back(code);
    if (codes.empty()) {
        std::cout << "No codes in " << path << "\n";
        return;
    }

    std::unordered_map<std::string, Airport> byCode;
    int total = (int)airports.size();
    for (int i = 0; i < total; i++) byCode[airports[i].code] = airports[i];
    std::vector<Airport> current = tree.inorderAirports();
    int m = (int)current.size();
    for (int i = 0; i < m; i++) byCode[current[i].code] = current[i];

    bool hadTree = tree.size() > 0;
    int oldHeight = tree.height();
    std::vector<std::string> oldLevels = tree.levelOrderCodes();

    tree.clear();
    int missing = 0;
    int n = (int)codes.size();
    for (int i = 0; i < n; i++) {
        std::unordered_map<std::string, Airport>::const_iterator it = byCode.find(codes[i]);
        if (it == byCode.end()) missing++;
        else tree.insert(it->second);
    }
    std::cout << "Loaded " << tree.size() << " airports from " << path;
    if (missing > 0) std::cout << " (" << missing << " codes not found)";
    std::cout << "\n";
    printShape(tree);
    printLevels(tree);
    if (hadTree) {
        std::cout << "Same height as before: " << (tree.height() == oldHeight ? "YES" : "NO")
                  << "   Same level order: " << (tree.levelOrderCodes() == oldLevels ? "YES" : "NO") << "\n";
    }
}

// Handles "range FROM TO": lists every airport with FROM <= code <= TO and the nodes visited.
void doRange(const AirportBST& tree, const std::string& from, const std::string& to) {
    if (from.empty() || to.empty()) {
        std::cout << "Usage: range FROM TO\n";
        return;
    }
    std::vector<Airport> found;
    int visited = tree.range(from, to, found);
    int n = (int)found.size();
    std::cout << n << " airports, " << visited << " nodes visited\n";
    for (int i = 0; i < n; i++) printAirport(found[i]);
}

// Builds a tree by inserting the airports one by one in the given order.
void buildInOrder(AirportBST& tree, const std::vector<Airport>& order) {
    tree.clear();
    int n = (int)order.size();
    for (int i = 0; i < n; i++) tree.insert(order[i]);
}

// Searches for every code in 'queries' and returns the time in milliseconds
// (the best of 5 runs, to reduce noise). 'found' gets the number of codes found.
double timeSearches(const AirportBST& tree, const std::vector<std::string>& queries, int& found) {
    double best = -1.0;
    int q = (int)queries.size();
    for (int run = 0; run < 5; run++) {
        found = 0;
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
        for (int i = 0; i < q; i++) {
            if (tree.contains(queries[i])) found++;
        }
        std::chrono::steady_clock::time_point stop = std::chrono::steady_clock::now();
        double ms = std::chrono::duration<double, std::milli>(stop - start).count();
        if (best < 0.0 || ms < best) best = ms;
    }
    return best;
}

// Prints one row of the Task 6 table.
void printRow(const std::string& label, const AirportBST& tree, const std::vector<std::string>& queries) {
    int found = 0;
    double ms = timeSearches(tree, queries, found);
    std::cout << std::left << std::setw(24) << label << std::right
              << std::setw(8) << tree.height()
              << std::setw(14) << std::fixed << std::setprecision(1) << tree.averageDepth()
              << std::setw(24) << std::setprecision(3) << ms
              << (found == (int)queries.size() ? "" : "   (some codes not found!)") << "\n";
}

// Handles "experiment": builds the tree in four orders from the loaded airports and
// prints height, average depth and the time for 10,000 random searches for each.
void doExperiment(const std::vector<Airport>& airports) {
    int n = (int)airports.size();
    if (n == 0) {
        std::cout << "Load the airports first: load data/airports.csv\n";
        return;
    }
    std::mt19937 rng(2560);
    std::vector<std::string> queries;
    for (int i = 0; i < 10000; i++) queries.push_back(airports[rng() % (unsigned)n].code);

    std::cout << std::left << std::setw(24) << "Build order" << std::right
              << std::setw(8) << "Height" << std::setw(14) << "Avg depth"
              << std::setw(24) << "10,000 searches (ms)" << "\n";

    AirportBST tree;
    buildInOrder(tree, airports);
    printRow("File order", tree, queries);

    std::vector<Airport> sorted = airports;
    std::sort(sorted.begin(), sorted.end(), [](const Airport& a, const Airport& b) { return a.code < b.code; });
    buildInOrder(tree, sorted);
    printRow("Sorted order", tree, queries);

    tree.buildBalanced(sorted);
    printRow("Balanced (middle rule)", tree, queries);

    std::vector<Airport> shuffled = airports;
    std::shuffle(shuffled.begin(), shuffled.end(), rng);
    buildInOrder(tree, shuffled);
    printRow("Random shuffle (bonus)", tree, queries);
}

int main() {
    AirportBST tree;
    std::vector<Airport> airports;
    std::string line;

    std::cout << "> ";
    while (std::getline(std::cin, line)) {
        std::istringstream words(line);
        std::string cmd;
        words >> cmd;

        if (cmd == "load") {
            std::string path;
            words >> path;
            int skipped = 0;
            if (!loadAirports(path, airports, skipped)) {
                std::cout << "Cannot open file: " << path << "\n";
            } else {
                tree.clear();
                int duplicates = 0;
                int n = (int)airports.size();
                for (int i = 0; i < n; i++) {
                    if (!tree.insert(airports[i])) {
                        duplicates++;
                        std::cout << "Duplicate code: " << airports[i].code << "\n";
                    }
                }
                std::cout << "Loaded " << tree.size() << " airports (" << skipped
                          << " lines skipped: no IATA code). Duplicates: " << duplicates << "\n";
                printShape(tree);
            }
        } else if (cmd == "check") {
            std::vector<std::string> codes = tree.inorderCodes();
            int n = (int)codes.size();
            bool sorted = true;
            for (int i = 1; i < n; i++) {
                if (!(codes[i - 1] < codes[i])) sorted = false;
            }
            std::cout << "Inorder list: " << n << " codes";
            if (n > 0) std::cout << ", first " << codes[0] << ", last " << codes[n - 1];
            std::cout << ", sorted: " << (sorted ? "YES" : "NO") << "\n";
        } else if (cmd == "find") {
            std::string code;
            words >> code;
            doFind(tree, code);
        } else if (cmd == "minmax") {
            Airport lo, hi;
            if (tree.minAirport(lo) && tree.maxAirport(hi)) {
                std::cout << "Minimum: ";
                printAirport(lo);
                std::cout << "Maximum: ";
                printAirport(hi);
            } else {
                std::cout << "The tree is empty.\n";
            }
        } else if (cmd == "remove") {
            std::string code;
            words >> code;
            doRemove(tree, code);
        } else if (cmd == "add") {
            std::string code, rest;
            words >> code;
            std::getline(words, rest);
            if (!rest.empty() && rest[0] == ' ') rest = rest.substr(1);
            doAdd(tree, code, rest);
        } else if (cmd == "levels") {
            printLevels(tree);
        } else if (cmd == "validate") {
            printValidate(tree);
        } else if (cmd == "list") {
            int limit = -1;
            int value;
            if (words >> value) limit = value;
            doList(tree, limit);
        } else if (cmd == "save") {
            std::string path;
            words >> path;
            doSave(tree, path);
        } else if (cmd == "loadtree") {
            std::string path;
            words >> path;
            doLoadTree(tree, airports, path);
        } else if (cmd == "range") {
            std::string from, to;
            words >> from >> to;
            doRange(tree, from, to);
        } else if (cmd == "rebalance") {
            tree.rebalance();
            printShape(tree);
        } else if (cmd == "experiment") {
            doExperiment(airports); 
        } else if (cmd == "quit" || cmd == "exit") {
            break;
        } else if (!cmd.empty()) {
            std::cout << "Unknown command. Try: load, check, find, minmax, remove, add, levels, validate, list, save, loadtree, range, rebalance, experiment, quit.\n";
        }
        std::cout << "> ";
    }
    std::cout << "\n";
    return 0;
}