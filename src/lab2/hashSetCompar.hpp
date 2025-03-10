#ifndef HASHSET_COMPARER_H
#define HASHSET_COMPARER_H

#include <unordered_set>
#include <string>
#include <vector>
#include <algorithm>
#include <functional>

struct HashSetComparer {
    bool operator()(const std::unordered_set<std::string>& x, const std::unordered_set<std::string>& y) const {
        return x == y; // Check if sets contain the same elements
    }

    std::size_t operator()(const std::unordered_set<std::string>& obj) const {
        std::vector<std::string> sortedElements(obj.begin(), obj.end());
        std::sort(sortedElements.begin(), sortedElements.end()); // Ensure consistent order

        std::size_t hash = 0;
        for (const auto& s : sortedElements) {
            hash ^= std::hash<std::string>{}(s) + 0x9e3779b9 + (hash << 6) + (hash >> 2); // Hash combination
        }
        return hash;
    }
};

#endif
