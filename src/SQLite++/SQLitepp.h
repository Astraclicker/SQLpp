#pragma once

#include <sqlite3.h>

#include <string>
#include <vector>

#include <defSqlCommon.h>
#include <defSQLite.h>
#include <json.hpp>

namespace astra_sql {
class SQLitepp
{
protected:
    // sqlite执行命令
    std::string cmd;
    // sqlite错误处理
    int checkError{};
    char *errMsg = nullptr;
    // sqlite执行接口
    sqlite3_stmt *stmt = nullptr;
    // sqlite数据库接口
    sqlite3 *db{};

    // 释放当前预处理语句，避免句柄被覆盖而泄漏
    void resetStatement() {
        if (stmt != nullptr) {
            sqlite3_finalize(stmt);
            stmt = nullptr;
        }
    }

    // 连接是否已打开
    bool isOpen() const {
        return db != nullptr;
    }

public:
    // 句柄由本类独占，禁止拷贝，避免析构时重复释放
    SQLitepp(const SQLitepp &) = delete;

    SQLitepp &operator=(const SQLitepp &) = delete;

    SQLitepp(SQLitepp &&) = delete;

    SQLitepp &operator=(SQLitepp &&) = delete;

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
        errorCode.clear();
        if (sqlite3_open(dbName.c_str(), &db) != SQLITE_OK) {
            errorCode = db != nullptr ? sqlite3_errmsg(db) : "sqlite3_open failed";
            if (db != nullptr) {
                sqlite3_close(db);
                db = nullptr;
            }
            return;
        }
        // 外键开关只能在连接成功之后设置
        if (foreign_key) {
            checkError = sqlite3_exec(db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, &errMsg);
            if (checkError != SQLITE_OK) {
                errorCode = sqlite3_errmsg(db);
                sqlite3_free(errMsg);
                errMsg = nullptr;
                sqlite3_close(db);
                db = nullptr;
                return;
            }
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
        const std::vector<sqliteCreateTableRule> &createRule,
        const primaryKeyRule *primaryKey,
        const uniqueKeyRule *uniqueKey,
        std::string &errorCode,
        callbackFunc &&callbackSuccess
    ) {
        errorCode.clear();
        if (!isOpen()) {
            errorCode = "sqlite database is not open";
            return;
        }
        cmd = "create table if not exists " + tableName + " ( ";
        for (const auto &i : createRule) {
            cmd += i.field;
            cmd += ' ';
            cmd += toSqliteDataType(i.type);
            cmd += ' ';
            cmd += i.restriction;
            cmd += ',';
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
            callbackSuccess();
        } catch (std::exception &error) {
            errorCode = error.what();
        }
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
        errorCode.clear();
        if (!isOpen()) {
            errorCode = "sqlite database is not open";
            return;
        }
        cmd = "drop table if exists " + tableName;
        try {
            checkError = sqlite3_exec(db, cmd.c_str(), nullptr, nullptr, &errMsg);
            if (checkError != SQLITE_OK) {
                throw std::runtime_error(sqlite3_errmsg(db));
            }
            callbackSuccess();
        } catch (std::exception &error) {
            errorCode = error.what();
        }
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
        errorCode.clear();
        if (!isOpen()) {
            errorCode = "sqlite database is not open";
            return;
        }
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
        resetStatement();
        try {
            checkError = sqlite3_prepare_v2(db, cmd.c_str(), -1, &stmt, nullptr);
            if (checkError != SQLITE_OK) {
                throw std::runtime_error(sqlite3_errmsg(db));
            }
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
                    case sqliteDataType::Text:
                        sqlite3_bind_text(stmt, i + 1, data[i].second.c_str(), -1, SQLITE_STATIC);
                        break;
                }
            }
            checkError = sqlite3_step(stmt);
            if (checkError != SQLITE_DONE) {
                throw std::runtime_error(sqlite3_errmsg(db));
            }
            resetStatement();
            callbackSuccess();
        } catch (std::exception &error) {
            errorCode = error.what();
            resetStatement();
        }
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
        errorCode.clear();
        if (!isOpen()) {
            errorCode = "sqlite database is not open";
            return;
        }
        // 准备语句
        cmd = "delete from " + tableName;
        for (auto i = rule.begin(); i != rule.end(); ++i) {
            cmd += i == rule.begin() ? " where" : " " + i->link;
            cmd += " " + i->field + " ";
            cmd += toSqlOp(i->op);
            cmd += '?';
        }
        cmd += ';';
        const auto cnt = rule.size();
        resetStatement();
        try {
            checkError = sqlite3_prepare_v2(db, cmd.c_str(), -1, &stmt, nullptr);
            if (checkError != SQLITE_OK) {
                throw std::runtime_error(sqlite3_errmsg(db));
            }
            // 绑定参数
            for (int i = 0; i < cnt; i++) {
                sqlite3_bind_text(stmt, i + 1, rule[i].value.c_str(), -1, SQLITE_STATIC);
            }
            // 执行语句
            checkError = sqlite3_step(stmt);
            if (checkError != SQLITE_DONE) {
                throw std::runtime_error(sqlite3_errmsg(db));
            }
            resetStatement();
            callbackSuccess();
        } catch (std::exception &error) {
            errorCode = error.what();
            resetStatement();
        }
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
        errorCode.clear();
        if (!isOpen()) {
            errorCode = "sqlite database is not open";
            return;
        }
        // 准备语句
        cmd = "update " + tableName + " set ";
        for (auto i = data.begin(); i != data.end(); ++i) {
            cmd += i == (data.end() - 1) ? i->first + " =?" : i->first + " =?,";
        }
        for (auto i = rule.begin(); i != rule.end(); ++i) {
            cmd += i == rule.begin() ? " where" : " " + i->link;
            cmd += " " + i->field + " ";
            cmd += toSqlOp(i->op);
            cmd += '?';
        }
        const auto cnt_data = data.size();
        const auto cnt_rule = rule.size();
        resetStatement();
        try {
            checkError = sqlite3_prepare_v2(db, cmd.c_str(), -1, &stmt, nullptr);
            if (checkError != SQLITE_OK) {
                throw std::runtime_error(sqlite3_errmsg(db));
            }
            // 绑定参数
            for (auto i = 0; i < cnt_data; i++) {
                sqlite3_bind_text(stmt, i + 1, data[i].second.c_str(), -1, SQLITE_STATIC);
            }
            for (auto i = 0; i < cnt_rule; i++) {
                sqlite3_bind_text(stmt, i + cnt_data + 1, rule[i].value.c_str(), -1, SQLITE_STATIC);
            }
            // 执行语句
            checkError = sqlite3_step(stmt);
            if (checkError != SQLITE_DONE) {
                throw std::runtime_error(sqlite3_errmsg(db));
            }
            resetStatement();
            callbackSuccess();
        } catch (std::exception &error) {
            errorCode = error.what();
            resetStatement();
        }
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
    nlohmann::json sqliteSearchItem(
        const std::string &tableName,
        const std::vector<std::string> &data,
        const itemRule &rule,
        std::string &errorCode,
        callbackFunc &&callbackSuccess
    ) {
        errorCode.clear();
        if (!isOpen()) {
            errorCode = "sqlite database is not open";
            return nlohmann::json{};
        }
        // 准备语句
        cmd = "select ";
        for (auto i = data.begin(); i != data.end(); ++i) {
            cmd += i == data.begin() ? *i : "," + *i;
        }
        cmd += " from " + tableName;
        for (auto i = rule.begin(); i != rule.end(); ++i) {
            cmd += i == rule.begin() ? " where" : " " + i->link;
            cmd += " " + i->field + " ";
            cmd += toSqlOp(i->op);
            cmd += '?';
        }
        // 执行语句,写入结果
        const auto cnt_data = data.size();
        nlohmann::json result = nlohmann::json::object();
        for (auto i = 0; i < cnt_data; i++) {
            result[data[i]] = nlohmann::json::array();
        }

        resetStatement();
        try {
            checkError = sqlite3_prepare_v2(db, cmd.c_str(), -1, &stmt, nullptr);
            if (checkError != SQLITE_OK) {
                throw std::runtime_error(sqlite3_errmsg(db));
            }
            // 绑定参数
            const auto cnt_rule = rule.size();
            for (auto i = 0; i < cnt_rule; i++) {
                sqlite3_bind_text(stmt, i + 1, rule[i].value.c_str(), -1, SQLITE_STATIC);
            }
            while ((checkError = sqlite3_step(stmt)) == SQLITE_ROW) {
                for (auto i = 0; i < cnt_data; i++) {
                    // 该列为 NULL 时 sqlite3_column_text 返回空指针
                    const auto *text = sqlite3_column_text(stmt, i);
                    if (text == nullptr) {
                        result[data[i]].push_back(nlohmann::json(nullptr));
                    } else {
                        result[data[i]].push_back(reinterpret_cast<const char *>(text));
                    }
                }
            }

            if (checkError != SQLITE_DONE) {
                throw std::runtime_error(sqlite3_errmsg(db));
            }
            resetStatement();
            callbackSuccess();
        } catch (std::exception &error) {
            errorCode = error.what();
            resetStatement();
            return nlohmann::json{};
        }
        return result;
    }

    // 析构
    ~SQLitepp() {
        resetStatement();
        if (db != nullptr) {
            sqlite3_close(db);
            db = nullptr;
        }
        sqlite3_free(errMsg);
        errMsg = nullptr;
    }
};
} // namespace astra_sql