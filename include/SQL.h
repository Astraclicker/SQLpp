#pragma once

#include <string>
#include <vector>

namespace astra_sql {
// SQL默认端口
constexpr unsigned int MySQL_DEFAULT_PORT = 3306;
constexpr unsigned int Redis_DEFAULT_PORT = 6379;

// mysql数据类型枚举
enum class mysqlDataType
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
};

// sqlite数据类型枚举
enum class sqliteDataType
{
    Blob,
    Double,
    Int,
    Int64,
    Null,
    Text,
};

// sql符号正则
enum class sqlCompareOp
{
    Eq, Ne, Lt, Gt, Le, Ge, Like, NotLike, In, NotIn, Is, IsNot
};

inline std::string_view toSql(sqlCompareOp op) {
    switch (op) {
        case sqlCompareOp::Eq:
            return "=";
        case sqlCompareOp::Ne:
            return "<>";
        case sqlCompareOp::Lt:
            return "<";
        case sqlCompareOp::Gt:
            return ">";
        case sqlCompareOp::Le:
            return "<=";
        case sqlCompareOp::Ge:
            return ">=";
        case sqlCompareOp::Like:
            return "LIKE";
        case sqlCompareOp::NotLike:
            return "NOT LIKE";
        case sqlCompareOp::In:
            return "IN";
        case sqlCompareOp::NotIn:
            return "NOT IN";
        case sqlCompareOp::Is:
            return "IS";
        case sqlCompareOp::IsNot:
            return "IS NOT";
    }
    return "="; // 不可达
}

// sql searchRule规则
struct searchRule
{
    std::string field;        // 表头
    sqlCompareOp op;          // 符号正则
    std::string value;        // 插入值
    std::string link = "AND"; // 连接词
};

// sql 创表规则
struct createTableRule
{
    std::string field;       // 表头
    std::string type;        // 数据类型
    std::string restriction; // 列约束
};

// 创表key规则
using primaryKeyRule = std::vector<std::string>;
using uniqueKeyRule = std::vector<std::string>;
// sql表增加数据参数
using item = std::vector<std::pair<std::string, std::string> >;
// sql表条件参数
using itemRule = std::vector<searchRule>;
// mysql表数据类型参数
using mysqlItemType = std::vector<mysqlDataType>;
// sqlite表数据类型参数
using sqliteItemType = std::vector<sqliteDataType>;
} // namespace astra_sql