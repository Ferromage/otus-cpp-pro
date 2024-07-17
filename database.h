#pragma once

#include <string>
#include <map>

class IDatabase {
public:
    virtual ~IDatabase() = default;
    virtual std::string insert(const std::string& table, const std::string& id, const std::string& name) = 0;
    virtual std::string truncate(const std::string& table) = 0;
    virtual std::string intersection() = 0;
    virtual std::string difference() = 0;
};


class CustomDatabase : public IDatabase {
public:
    CustomDatabase() = default;
    std::string insert(const std::string& table, const std::string& id, const std::string& name) override;
    std::string truncate(const std::string& table) override;
    std::string intersection() override;
    std::string difference() override;

private:
    std::map<int, std::string> tableA_;
    std::map<int, std::string> tableB_;
};