#pragma once
#include <string>
#include <vector>

namespace astra_sql {
// sql符号正则
enum class sqlCompareOp
{
    Eq, Ne, Lt, Gt, Le, Ge, Like, NotLike, In, NotIn, Is, IsNot
};

// sql符号正则转换到string
inline std::string_view toSqlOp(sqlCompareOp op) {
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

// 创表key规则
using primaryKeyRule = std::vector<std::string>;
using uniqueKeyRule = std::vector<std::string>;
// sql表增加数据参数
using item = std::vector<std::pair<std::string, std::string> >;
// sql表条件参数
using itemRule = std::vector<searchRule>;
}