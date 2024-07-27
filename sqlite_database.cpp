#include "database.h"
#include "sqlite3.h"

#include <iostream>
#include <stdexcept>
#include <sstream>
#include <map>
#include <functional>

using namespace std::string_literals;

namespace {
    std::map<std::string, std::pair<std::string, std::string>> selectionTable;
}

class SQLiteDatabase : public IDatabase {
public:
    SQLiteDatabase() {
        if (sqlite3_open("mydb.sqlite", &handle_)) {
            const std::string errMsg = "Can't open database: "s + sqlite3_errmsg(handle_);
            sqlite3_close(handle_);
            throw std::runtime_error(errMsg);
        }
        
        truncate("A");
        truncate("B");
        
        {
            const auto res = executeSQL("CREATE TABLE A (id INT PRIMARY KEY, name VARCHAR(255));", nullptr);
            if (!res.first) {
                throw std::runtime_error("Cannot create table A");
            }
        }

        {
            const auto res = executeSQL("CREATE TABLE B (id INT PRIMARY KEY, name VARCHAR(255));", nullptr);
            if (!res.first) {
                throw std::runtime_error("Cannot create table B");
            }
        }
    }

    ~SQLiteDatabase() override {
        if (handle_) {
            sqlite3_close(handle_);
        }
    }

    std::string insert(const std::string& table, const std::string& id, const std::string& name) override {
        if (table.empty() || id.empty() || name.empty()) {
            return ERROR_PREFIX + " wrong parameters\n";
        }

        if (table != "A" && table != "B") {
            return ERROR_PREFIX + " unknown table\n";
        }

        const std::string sql = "INSERT INTO " + table + " VALUES (" + id + ", \"" + name + "\");";
        const auto res = executeSQL(sql, nullptr);
        if (res.first) {
            return SUCCESS_PREFIX + '\n';
        } else {
            return ERROR_PREFIX + " " + res.second + '\n';
        }
    }

    std::string truncate(const std::string& table) override {
        if (table.empty()) {
            return ERROR_PREFIX + " wrong parameter\n";
        }

        if (table != "A" && table != "B") {
            return ERROR_PREFIX + " unknown table\n";
        }
                
        const auto res = executeSQL("DROP TABLE " + table + ";", nullptr);
        if (res.first) {
            return SUCCESS_PREFIX + '\n';
        } else {
            return ERROR_PREFIX + " " + res.second + '\n';
        }
    }

    std::string intersection() override {
        selectionTable.clear();

        auto print_results = [](void *, int columns, char **data, [[maybe_unused]] char **names) -> int {
            std::vector<std::string> line;
            for (int i = 0; i < std::min(columns, 3); ++i) {
                line.emplace_back((data[i] ? data[i] : ""));
            }

            selectionTable[line.at(0)] = {line.at(1), line.at(2)};
            return 0;
        };

        const auto res = executeSQL("SELECT A.id, A.name, B.name FROM A JOIN B ON A.id = B.id;", print_results);
        if (res.first) {
            std::ostringstream oss;
            for (const auto &[id, names] : selectionTable) {
                oss << id << "," << names.first << "," << names.second << "\n";
            }
            return oss.str() + SUCCESS_PREFIX + '\n';
        } else {
            return ERROR_PREFIX + " " + res.second + '\n';
        }
    }

    std::string difference() override {
        selectionTable.clear();

        auto print_results1 = [](void *, int columns, char **data, [[maybe_unused]] char **names) -> int {
            std::vector<std::string> line;
            for (int i = 0; i < std::min(columns, 2); ++i) {
                line.emplace_back((data[i] ? data[i] : ""));
            }

            selectionTable[line.at(0)] = {line.at(1), ""};
            return 0;
        };

        auto print_results2 = [](void *, int columns, char **data, [[maybe_unused]] char **names) -> int {
            std::vector<std::string> line;
            for (int i = 0; i < std::min(columns, 2); ++i) {
                line.emplace_back((data[i] ? data[i] : ""));
            }

            selectionTable[line.at(0)] = {"", line.at(1)};
            return 0;
        };

        const auto res1 = executeSQL("SELECT A.id, A.name FROM A LEFT OUTER JOIN B ON A.id = B.id WHERE B.id IS NULL;", print_results1);
        const auto res2 = executeSQL("SELECT B.id, B.name FROM B LEFT OUTER JOIN A ON B.id = A.id WHERE A.id IS NULL;", print_results2);
        if (res1.first && res2.first) {
            std::ostringstream oss;
            for (const auto &[id, names] : selectionTable) {
                oss << id << "," << names.first << "," << names.second << '\n';
            }
            return oss.str() + SUCCESS_PREFIX + '\n';
        } else {
            return ERROR_PREFIX + " " + (res1.second.empty() ? res2.second : res1.second) + '\n';
        }
    }

private:
    template<typename Callback>
    std::pair<bool, std::string> executeSQL(const std::string& sql, Callback callback) {
        char *errMsg;
        if (sqlite3_exec(handle_, sql.c_str(), callback, 0, &errMsg) != SQLITE_OK) {
            std::cerr << "Can't execute query: " << errMsg << std::endl;
            sqlite3_free(errMsg);
            return {false, errMsg};
        }
        return {true, ""};
    }
    
    sqlite3* handle_ = nullptr;
};


std::unique_ptr<IDatabase> getSQLiteDatabase() {
    return std::make_unique<SQLiteDatabase>();
}