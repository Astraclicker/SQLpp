#pragma once

#include <sqlite3.h>
#include <string>

#include <SQL.h>

namespace astra_sql {
    class SQLitepp {
    protected:
        std::string cmd;
        char *errMsg = nullptr;
        sqlite3 *db{};

    public:
        /**
         * @brief 构造函数
         * @param dbName 数据库名称
         * @param foreign_key 是否启用外键
         */
        SQLitepp(const std::string &dbName, bool foreign_key);

        /**
         * @brief sqlite创建表
         * @param tableName 要创建的表名
         * @param createRule 创建表的规则
         * @param primaryKey 主键规则
         * @param uniqueKey 为唯一键规则
         * @return 报错枚举
         */
        SQLppError sqliteCreateTable
        (
            const std::string &tableName,
            const std::vector<createTableRule> &createRule,
            const primaryKeyRule &primaryKey,
            const uniqueKeyRule &uniqueKey
        );

        ~SQLitepp();
    };
}
