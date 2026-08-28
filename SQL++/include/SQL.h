#pragma once

#include <array>
#include <vector>
#include <string>
namespace astra_sql
{
    // MySQL默认端口
    constexpr unsigned int MySQL_DEFAULT_PORT = 3306;
    constexpr unsigned int Redis_DEFAULT_PORT = 6379;

    // mysql数据类型枚举
    enum class dataType
    {
        BigInt,   // string
        Blob,     // istream
        Bool,     // bool
        DataTime, // stream
        Double,   // double
        Int32,    // int32_t
        Int64,    // int64_t
        Null,     // int
        String,   // string
        Uint32,   // uint32_t
        Uint64,   // uint64_t
        Vector,   // std::vector
    };

    // mysql函数返回值枚举
    enum class MySQLppError
    {
        error_database,
        error_create,
        error_del,
        error_change,
        error_search,
        success,
    };

    // sql searchRule规则
    struct searchRule
    {
        std::string field;        // 表头
        std::string op;           // "=" "!=" ">" "<" ">=" "<=" "LIKE" ... 判断正则
        std::string value;        // "root"
        std::string link = "AND"; // 连接词
    };

    // mysql表增加数据参数
    using item = std::vector<std::pair<std::string, std::string>>;
    // mysql表条件参数
    using itemRule = std::vector<searchRule>;
    // mysql表数据类型参数
    using itemType = std::vector<dataType>;
}
