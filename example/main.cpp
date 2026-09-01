#include <Redis++/Redispp.h>


void example_redis() {
    std::string password = "1108372699a";
    astra_sql::Redispp redis("139.129.35.66", astra_sql::Redis_DEFAULT_PORT, nullptr, &password, 0);
}

int main() {
    example_redis();
    return 0;
}
