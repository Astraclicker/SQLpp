#include <SQLite++/SQLitepp.h>

#include <fstream>

void example_sqlite() {
    //创建数据库并开启外键
    astra_sql::SQLitepp sqlite("_test.db", true);

    //删除表
    //sqlite.sqliteDelTable("users");

    //创建表
    std::vector<astra_sql::createTableRule> rule_test{
        {"uid", "INTEGER", "primary key AUTOINCREMENT"},
        {"userName", "string", "not null unique"},
        {"password", "string", "not null"}
    };
    sqlite.sqliteCreateTable("users", rule_test, nullptr, nullptr);


    //向表中插入数据
    astra_sql::item testItem{
        {"userName", "root"},
        {"password", "114514"}
    };
    astra_sql::sqliteItemType testType{
        astra_sql::sqliteDataType::Text,
        astra_sql::sqliteDataType::Text
    };
    sqlite.sqliteInsertItem("users", testItem, testType);

    // 删除表中数据
    astra_sql::itemRule testRule{
        {"userName", "=", "root"},
        {"uid", ">", "0", "and"},
    };
    sqlite.sqliteDelItem("users", testRule);

    //修改表中数据
    // astra_sql::item item_test{
    //     {"userName", "xn王玉玺"},
    //     {"password", "0721"},
    // };
    // astra_sql::itemRule rule_test{
    //     {"userName", "=", "王玉玺"},
    //     {"uid", ">", "0", "and"},
    // };
    //sqlite.sqliteUpdateItem("users", item_test, rule_test);

    //查询表中数据
    std::vector<std::string> data_test{
        "userName",
        "password",
    };
    astra_sql::itemRule test_rule
    {
        {"uid", ">", "0", "and"},
    };
    std::ofstream file("temp.json");
    file << sqlite.sqlitSearchItem("users", data_test, test_rule);
}

int main() {
    example_sqlite();
    return 0;
}
