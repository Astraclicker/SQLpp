#include <SQLite++/SQLitepp.h>

#include <fstream>

int main() {
    astra_sql::SQLitepp sqlite("test.db", true);

    const std::vector<astra_sql::createTableRule> rule
    {
        {"uid", "int", "not null"},
        {"userName", "varchar(50)", "not null"},
        {"password", "varchar(50)", "not null"}
    };
    sqlite.sqliteCreateTable("users", rule, {"uid"}, {"uid", "userName"});
    return 0;
}
