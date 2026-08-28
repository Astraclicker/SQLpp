#include "SQLitepp.h"

SQLitepp::SQLitepp(const std::string &dbName)
{
    if (sqlite3_open(dbName.c_str(), &db) != SQLITE_OK)
    {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
    }
    std::clog << "SQLite connect successfully" << std::endl;
}

SQLitepp::~SQLitepp()
{
    sqlite3_close(db);
}
