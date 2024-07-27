#pragma once

#include <string>
#include <memory>

static const std::string SUCCESS_PREFIX = "OK";
static const std::string ERROR_PREFIX = "ERR";

class IDatabase {
public:
    virtual ~IDatabase() = default;
    virtual std::string insert(const std::string& table, const std::string& id, const std::string& name) = 0;
    virtual std::string truncate(const std::string& table) = 0;
    virtual std::string intersection() = 0;
    virtual std::string difference() = 0;
};

std::unique_ptr<IDatabase> getCustomDatabase();
std::unique_ptr<IDatabase> getSQLiteDatabase();