#include "Redispp.h"

namespace astra_sql
{
    Redispp::Redispp(const std::string &hostName, unsigned int port, const std::string &password, unsigned int db)
    {

        opts = new sw::redis::ConnectionOptions;
        opts->host = hostName;
        opts->port = port;
        opts->password = password;
        opts->db = db;

        try
        {
            redis = new sw::redis::Redis(*opts);
            redis->ping();
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
            return;
        }

        std::clog << "Redis connect successfully" << std::endl;
    }

    Redispp::Redispp(const std::string &hostName, unsigned int port, const std::string &userName, const std::string &password, unsigned int db)
    {
        opts->host = hostName;
        opts->port = port;
        opts->user = userName;
        opts->password = password;
        opts->db = db;

        try
        {
            redis = new sw::redis::Redis(*opts);
            redis->ping();
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
            return;
        }
        std::clog << "Redis connect successfully" << std::endl;
    }
}