#pragma once

#include <vector>

namespace astra_sql {
// sqlite数据类型枚举
enum class sqliteDataType
{
    Blob,
    Double,
    Int,
    Text,
};

// sqlite数据类型转换为string
inline std::string_view toSqliteDataType(const sqliteDataType dataType) {
    switch (dataType) {
        case sqliteDataType::Blob: {
            return "Blob";
        }
        case sqliteDataType::Double: {
            return "Real";
        }
        case sqliteDataType::Int: {
            return "INTEGER";
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