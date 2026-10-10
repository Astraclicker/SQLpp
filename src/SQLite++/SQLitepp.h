#pragma once

#include <sqlite3.h>

#include <string>
#include <vector>
#include <mutex>

#include "../../include/SQL.h"
#include "../../include/json.hpp"

namespace astra_sql {
class SQLitepp
{
protected:
    //锁
    std::mutex sqlMtx;
    // sqlite执行命令
    std::string cmd;
    // sqlite错误处理
    int checkError{};
    char *errMsg = nullptr;
    // sqlite执行接口
    sqlite3_stmt *stmt = nullptr;
    // sqlite数据库接口
    sqlite3 *db{};

public:
    /**
     * @brief 构造函数
     * @param dbName 数据库名称
     * @param foreign_key 是否启用外键
     * @param errorCode 错误信息
     * @param callbackSuccess 连接成功回调函数
     */
    template <typename callbackFunc>
    SQLitepp(const std::string &dbName,
             const bool foreign_key,
             std::string &errorCode,
             callbackFunc &&callbackSuccess) {
        if (sqlite3_open(dbName.c_str(), &db) != SQLITE_OK) {
            errorCode = sqlite3_errmsg(db);
            if (foreign_key) {
                sqlite3_exec(db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);
            }
            return;
        }
        callbackSuccess();
    }

    /**
     * @brief sqlite创建表
     * @param tableName 要创建的表名
     * @param createRule 创建表的规则
     * @param primaryKey 主键规则
     * @param uniqueKey 为唯一键规则
     * @param errorCode 错误信息
     * @param callbackSuccess 连接成功回调函数
     */

    template <typename callbackFunc>
    void sqliteCreateTable(
        const std::string &tableName,
        const std::vector<createTableRule> &createRule,
        const primaryKeyRule *primaryKey,
        const uniqueKeyRule *uniqueKey,
        std::string &errorCode,
        callbackFunc &&callbackSuccess
    ) {
        std::unique_lock lock(sqlMtx);
        cmd = "create table if not exists " + tableName + " ( ";
        for (const auto &i : createRule) {
            cmd += i.field + " " + i.type + " " + i.restriction + ",";
        }
        if (primaryKey != nullptr) {
            cmd += "primary key (";
            for (auto i = primaryKey->begin(); i != primaryKey->end(); ++i) {
                cmd += i == primaryKey->begin() ? *i : "," + *i;
            }
            cmd += "),";
        }
        if (uniqueKey != nullptr) {
            cmd += "unique (";
            for (auto i = uniqueKey->begin(); i != uniqueKey->end(); ++i) {
                cmd += i == uniqueKey->begin() ? *i : "," + *i;
            }
            cmd += "),";
        }
        cmd.pop_back();
        cmd += " );";
        try {
            checkError = sqlite3_exec(db, cmd.c_str(), nullptr, nullptr, &errMsg);
            if (checkError != SQLITE_OK) {
                throw std::runtime_error(sqlite3_errmsg(db));
            }
        } catch (std::exception &error) {
            errorCode = error.what();
        }
        lock.unlock();
        callbackSuccess();
    }

    /**
     * @brief 删除表
     * @param tableName 要删除的表名
     * @return 报错枚举
     * @param errorCode 错误信息
     * @param callbackSuccess 连接成功回调函数
     * @warning 跑路啦兄弟，跑路啦
     */

    template <typename callbackFunc>
    void sqliteDelTable(
        const std::string &tableName,
        std::string &errorCode,
        callbackFunc &&callbackSuccess) {
        std::unique_lock lock(sqlMtx);
        cmd = "drop table if exists " + tableName;
        try {
            checkError = sqlite3_exec(db, cmd.c_str(), nullptr, nullptr, &errMsg);
            if (checkError != SQLITE_OK) {
                throw std::runtime_error(sqlite3_errmsg(db));
            }
        } catch (std::exception &error) {
            errorCode = error.what();
        }
        lock.unlock();
        callbackSuccess();
    }

    /**
     * @brief 向表中插入数据
     * @param tableName 表名
     * @param data 插入的数据
     * @param type 插入的数据的类型
     * @param errorCode 错误信息
     * @param callbackSuccess 连接成功回调函数
     */

    template <typename callbackFunc>
    void sqliteInsertItem(
        const std::string &tableName,
        const item &data,
        const sqliteItemType &type,
        std::string &errorCode,
        callbackFunc &&callbackSuccess) {
        std::unique_lock lock(sqlMtx);
        const auto cnt = data.size();
        cmd = "insert into " + tableName + "(";
        for (auto i = data.begin(); i != data.end(); ++i) {
            cmd += i == data.begin() ? i->first : "," + i->first;
        }
        cmd += ')';
        cmd += "values(";
        for (auto i = 0; i < cnt; i++) {
            cmd += i == 0 ? "?" : ",?";
        }
        cmd += ");";
        sqlite3_prepare_v2(db, cmd.c_str(), -1, &stmt, nullptr);

        for (int i = 0; i < cnt; i++) {
            switch (type[i]) {
                case sqliteDataType::Blob:
                    sqlite3_bind_blob(stmt, i + 1, data[i].second.c_str(), -1, SQLITE_STATIC);
                    break;
                case sqliteDataType::Double:
                    sqlite3_bind_double(stmt, i + 1, std::stod(data[i].second));
                    break;
                case sqliteDataType::Int:
                    sqlite3_bind_int(stmt, i + 1, std::stoi(data[i].second));
                    break;
                case sqliteDataType::Int64:
                    sqlite3_bind_int64(stmt, i + 1, std::stoll(data[i].second));
                    break;
                case sqliteDataType::Null:
                    sqlite3_bind_null(stmt, i + 1);
                    break;
                case sqliteDataType::Text:
                    sqlite3_bind_text(stmt, i + 1, data[i].second.c_str(), -1, SQLITE_STATIC);
                    break;
            }
        }
        try {
            checkError = sqlite3_step(stmt);
            if (checkError != SQLITE_DONE) {
                throw std::runtime_error(sqlite3_errmsg(db));
            }
        } catch (std::exception &error) {
            errorCode = error.what();
        }
        lock.unlock();
        callbackSuccess();
    }

    /**
     * @brief 删除表中数据
     * @param tableName 表名
     * @param rule 删除规则
     * @param errorCode 错误信息
     * @param callbackSuccess 连接成功回调函数
     */

    template <typename callbackFunc>
    void sqliteDelItem(
        const std::string &tableName,
        const itemRule &rule,
        std::string &errorCode,
        callbackFunc &&callbackSuccess) {
        std::unique_lock lock(sqlMtx);
        // 准备语句
        cmd = "delete from " + tableName;
        for (auto i = rule.begin(); i != rule.end(); ++i) {
            cmd += i == rule.begin() ? " where" : " " + i->link;
            cmd += " " + i->field + " " += toSql(i->op);
            cmd += '?';
        }
        cmd += ';';
        const auto cnt = rule.size();
        sqlite3_prepare_v2(db, cmd.c_str(), -1, &stmt, nullptr);
        // 绑定参数
        for (int i = 0; i < cnt; i++) {
            sqlite3_bind_text(stmt, i + 1, rule[i].value.c_str(), -1, SQLITE_STATIC);
        }
        // 执行语句
        try {
            checkError = sqlite3_step(stmt);
            if (checkError != SQLITE_DONE) {
                throw std::runtime_error(sqlite3_errmsg(db));
            }
        } catch (std::exception &error) {
            errorCode = error.what();
        }
        lock.unlock();
        callbackSuccess();
    }

    /**
     * @brief 更改表中数据
     * @param tableName 要更改的表名
     * @param data 更改的数据
     * @param rule 更改规则
     * @param errorCode 错误信息
     * @param callbackSuccess 连接成功回调函数
     */

    template <typename callbackFunc>
    void sqliteUpdateItem(
        const std::string &tableName,
        const item &data,
        const itemRule &rule,
        std::string &errorCode,
        callbackFunc &&callbackSuccess) {
        std::unique_lock lock(sqlMtx);
        // 准备语句
        cmd = "update " + tableName + " set ";
        for (auto i = data.begin(); i != data.end(); ++i) {
            cmd += i == (data.end() - 1) ? i->first + " =?" : i->first + " =?,";
        }
        for (auto i = rule.begin(); i != rule.end(); ++i) {
            cmd += i == rule.begin() ? " where" : " " + i->link;
            cmd += " " + i->field + " " += toSql(i->op);
            cmd += '?';
        }
        sqlite3_prepare_v2(db, cmd.c_str(), -1, &stmt, nullptr);
        const auto cnt_data = data.size();
        const auto cnt_rule = rule.size();
        // 绑定参数
        for (auto i = 0; i < cnt_data; i++) {
            sqlite3_bind_text(stmt, i + 1, data[i].second.c_str(), -1, SQLITE_STATIC);
        }
        for (auto i = 0; i < cnt_rule; i++) {
            sqlite3_bind_text(stmt, i + cnt_data + 1, rule[i].value.c_str(), -1, SQLITE_STATIC);
        }
        // 执行语句
        try {
            checkError = sqlite3_step(stmt);
            if (checkError != SQLITE_DONE) {
                throw std::runtime_error(sqlite3_errmsg(db));
            }
        } catch (std::exception &error) {
            errorCode = error.what();
        }
        lock.unlock();
        callbackSuccess();
    }

    /**
     * @brief 查找表中数据
     * @param tableName 要查找的表名
     * @param data 要查找的表头
     * @param rule 查找限制
     * @param errorCode 错误信息
     * @param callbackSuccess 连接成功回调函数
     * @return json格式的查找结果
     */

    template <typename callbackFunc>
    nlohmann::json sqlitSearchItem(
        const std::string &tableName,
        const std::vector<std::string> &data,
        const itemRule &rule,
        std::string &errorCode,
        callbackFunc &&callbackSuccess
    ) {
        std::unique_lock lock(sqlMtx);
        // 准备语句
        cmd = "select ";
        for (auto i = data.begin(); i != data.end(); ++i) {
            cmd += i == data.begin() ? *i : "," + *i;
        }
        cmd += " from " + tableName;
        for (auto i = rule.begin(); i != rule.end(); ++i) {
            cmd += i == rule.begin() ? " where" : " " + i->link;
            cmd += " " + i->field + " " += toSql(i->op);
            cmd += '/';
        }
        // 绑定参数
        sqlite3_prepare_v2(db, cmd.c_str(), -1, &stmt, nullptr);
        const auto cnt_rule = rule.size();
        for (auto i = 0; i < cnt_rule; i++) {
            sqlite3_bind_text(stmt, i + 1, rule[i].value.c_str(), -1, SQLITE_STATIC);
        }
        // 执行语句,写入结果
        const auto cnt_data = data.size();
        nlohmann::json result = nlohmann::json::object();
        for (auto i = 0; i < cnt_data; i++) {
            result[data[i]] = nlohmann::json::array();
        }

        try {
            while ((checkError = sqlite3_step(stmt)) == SQLITE_ROW) {
                for (auto i = 0; i < cnt_data; i++) {
                    result[data[i]].push_back(
                        reinterpret_cast<const char *>(sqlite3_column_text(stmt, i))
                    );
                }
            }

            if (checkError != SQLITE_DONE) {
                throw std::runtime_error(sqlite3_errmsg(db));
            }
        } catch (std::exception &error) {
            errorCode = error.what();
            return nlohmann::json{};
        }
        lock.unlock();
        callbackSuccess();
        return result;
    }

    // 析构
    ~SQLitepp() {
        sqlite3_close(db);
        sqlite3_free(errMsg);
        sqlite3_finalize(stmt);
    }
};
} // namespace astra_sql