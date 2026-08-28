#pragma once
#include <sqlite3.h>

#include <iostream>
#include <memory>
#include <string>

class SQLitepp
{
protected:
    sqlite3 *db;

public:
    SQLitepp(const std::string &dbName);
    ~SQLitepp();
};
