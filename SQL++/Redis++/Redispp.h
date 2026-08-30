#pragma once
#include <string>
#include<iostream>

#include <SQL.h>
#include <sw/redis++/redis++.h>

namespace astra_sql
{
    class Redispp
    {
    private:
        // Redis连接配置
        sw::redis::ConnectionOptions *opts;
        // Redis操作接口
        sw::redis::Redis *redis;

    public:
        Redispp(const std::string &hostName, unsigned int port, const std::string &password, unsigned int db);

        Redispp(const std::string &hostName, unsigned int port, const std::string &userName, const std::string &password, unsigned int db);
    };

}