#pragma once

#include <vector>

namespace astra_sql {
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

// sqlite数据类型转换为string
inline std::string_view toSqliteDataType(const sqliteDataType dataType) {
    switch (dataType) {
        case sqliteDataType::Blob: {
            return "Blob";
        }
        case sqliteDataType::Double: {
            return "Double";
        }
        case sqliteDataType::Int: {
            return "Int";
        }
        case sqliteDataType::Int64: {
            return "Int64";
        }
        case sqliteDataType::Null: {
            return "Null";
        }
        case sqliteDataType::Text: {
            return "Text";
        }
    }
    return "";
}

// sqlite表数据类型参数
using sqliteItemType = std::vector<sqliteDataType>;

// mysql创表规则
struct sqliteCreateTableRule
{
    std::string field;
    sqliteDataType type;
    std::string restriction;
};
}