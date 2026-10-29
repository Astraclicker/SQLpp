#pragma once

#include <vector>

namespace astra_sql {
// MySQL默认端口
constexpr uint16_t MySQL_DEFAULT_PORT = 3306;

// mysql数据类型枚举
enum class mysqlDataType
{
    BigInt,   // string
    Blob,     // istream
    Bool,     // bool
    DateTime, // stream
    Double,   // double
    Int32,    // int32_t
    Int64,    // int64_t
    Null,     // int
    String,   // string
    Uint32,   // uint32_t
    Uint64,   // uint64_t
};

// mysql数据类型转换到string
inline std::string_view toMySQLDataType(const mysqlDataType dataType) {
    switch (dataType) {
        case mysqlDataType::BigInt: {
            return "BigInt";
        }
        case mysqlDataType::Blob: {
            return "Blob";
        }
        case mysqlDataType::Bool: {
            return "Bool";
        }
        case mysqlDataType::DateTime: {
            return "DateTime";
        }
        case mysqlDataType::Double: {
            return "Double";
        }
        case mysqlDataType::Int32: {
            return "Int32";
        }
        case mysqlDataType::Int64: {
            return "Int64";
        }
        case mysqlDataType::Null: {
            return "Null";
        }
        case mysqlDataType::String: {
            return "Text";
        }
        case mysqlDataType::Uint32: {
            return "Uint32";
        }
        case mysqlDataType::Uint64: {
            return "Uint64";
        }
    }
    return "";
}

// mysql表数据类型参数
using mysqlItemType = std::vector<mysqlDataType>;

// mysql创表规则
struct mysqlCreateTableRule
{
    std::string field;
    mysqlDataType type;
    std::string restriction;
};
}