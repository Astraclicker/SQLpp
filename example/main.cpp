#include <MySQL++/MySQLpp.h>

#include <fstream>

void example_mysql()
{
    astra_sql::MySQLpp mysql("127.0.0.1", astra_sql::MySQL_DEFAULT_PORT, "root", "password");

    mysql.switchDatabase("test_db");

    astra_sql::itemRule rule{
        {"id", ">", "0"},
    };

    std::ofstream file("temp.json");

    file << mysql.searchItem("users", {"id", "userName", "password"}, rule);
}
int main()
{
    example_mysql();
    return 0;
}
