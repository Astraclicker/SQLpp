#include "SQLitepp.h"
#include <iostream>

namespace astra_sql {
    SQLitepp::SQLitepp(const std::string &dbName, const bool foreign_key) {
        if (sqlite3_open(dbName.c_str(), &db) != SQLITE_OK) {
            std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
            if (foreign_key) {
                sqlite3_exec(db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);
            }
            return;
        }
        std::clog << "SQLite connect successfully" << std::endl;
    }

    SQLppError SQLitepp::sqliteCreateTable
    (
        const std::string &tableName,
        const std::vector<createTableRule> &createRule,
        const primaryKeyRule &primaryKey,
        const uniqueKeyRule &uniqueKey
    ) {
        cmd = "create table if not exists " + tableName + " ( ";
        for (const auto &i: createRule) {
            cmd += i.field + " " + i.type + " " + i.restriction + ",";
        }
        if (!primaryKey.empty()) {
            cmd += "primary key (";
            for (auto i = primaryKey.begin(); i != primaryKey.end(); ++i) {
                cmd += i == primaryKey.begin() ? *i : "," + *i;
            }
            cmd += "),";
        }
        if (!uniqueKey.empty()) {
            cmd += "unique (";
            for (auto i = uniqueKey.begin(); i != uniqueKey.end(); ++i) {
                cmd += i == uniqueKey.begin() ? *i : "," + *i;
            }
            cmd += "),";
        }
        cmd.pop_back();
        cmd += " );";


        try {
            sqlite3_exec(db, cmd.c_str(), nullptr, nullptr, &errMsg);
        } catch (std::exception &error) {
            std::cerr << error.what() << std::endl << errMsg << std::endl;
            return SQLppError::error_create;
        }

        std::cout << cmd << std::endl;
        return SQLppError::success;
    }

    SQLitepp::~SQLitepp() {
        sqlite3_close(db);
        sqlite3_free(errMsg);
    }
}
