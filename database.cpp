#include "database.h"
#include <sstream>

using namespace std::string_literals;

namespace {
    static const std::string SUCCESS_PREFIX = "OK";
    static const std::string ERROR_PREFIX = "ERR";
}

std::string CustomDatabase::insert(const std::string& table, const std::string& id, const std::string& name) {
    if (table.empty() || id.empty() || name.empty()) {
        return ERROR_PREFIX + " wrong parameters\n";
    }

    std::map<int, std::string> *const ptable = (table == "A"s) ? &tableA_ : (table == "B"s) ? &tableB_ : nullptr;
    if (!ptable) {
        return ERROR_PREFIX + " unknown table\n";
    }

    auto p = ptable->emplace(std::stoi(id), name);
    if (!p.second) {
        return ERROR_PREFIX + " duplicate " + id + "\n";
    }
    
    return SUCCESS_PREFIX + '\n';
}

std::string CustomDatabase::truncate(const std::string& table) {
    if (table.empty()) {
        return ERROR_PREFIX + " wrong parameter\n";
    }

    std::map<int, std::string> *const ptable = (table == "A"s) ? &tableA_ : (table == "B"s) ? &tableB_ : nullptr;
    if (!ptable) {
        return ERROR_PREFIX + " unknown table\n";
    }

    if (ptable->empty()) {
        return ERROR_PREFIX + " already empty\n";
    }

    ptable->clear();
    return SUCCESS_PREFIX + '\n';
}

std::string CustomDatabase::intersection() {
    std::ostringstream oss;

    for (const auto& [id, name] : tableA_) {
        if (auto it = tableB_.find(id); it != tableB_.end()) {
            oss << id << ',' << name << ',' << it->second << '\n';
        }
    }
    return oss.str() + SUCCESS_PREFIX + '\n';
}

std::string CustomDatabase::difference() {
    std::map<int, std::pair<std::string, std::string>> table;

    for (const auto& [id, name] : tableA_) {
        if (auto it = tableB_.find(id); it == tableB_.end()) {
            table[id] = {name, {}};
        }
    }
    for (const auto& [id, name] : tableB_) {
        if (auto it = tableA_.find(id); it == tableA_.end()) {
            table[id] = {{}, name};
        }
    }
    
    std::ostringstream oss;
    for (const auto& [id, pair] : table) {
        oss << id << ',' << pair.first << ',' << pair.second << '\n';
    }
    return oss.str() + SUCCESS_PREFIX + '\n';
}
