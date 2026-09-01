#include <Redis++/Redispp.h>


void example_redis() {
    std::string password = "102410";
    astra_sql::Redispp redis("0.0.0.0", astra_sql::Redis_DEFAULT_PORT, nullptr, &password, 0);
}

int main() {
    example_redis();
    return 0;
}
