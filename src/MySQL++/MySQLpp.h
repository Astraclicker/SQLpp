#pragma once
#include <cppconn/connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/statement.h>
#include <mysql_driver.h>

#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <mutex>

#include "../../include/SQL.h"
#include "../../include/json.hpp"

namespace astra_sql {
class MySQLpp
{
protected:
    //锁
    std::mutex sqlMtx;
    // MySQL连接
    std::unique_ptr<sql::Connection> conn;
    // MySQL驱动
    sql::mysql::MySQL_Driver *driver;
    // MySQL执行接口
    std::unique_ptr<sql::PreparedStatement> preStmt;
    std::unique_ptr<sql::Statement> stmt;
    // 储存MySQL命令
    std::string cmd;

public:
    /**
     * @brief mysqlpp类构造函数
     * @param host MySQL服务器地址
     * @param port MySQL端口
     * @param UserName MySQL用户名
     * @param password MySQL密码
     * @param errorCode 错误信息
     * @param callbackSuccess 连接成功回调函数
     */

    template <typename callbackFunc>
    MySQLpp(
        const std::string &host,
        const int16_t port,
        const std::string &UserName,
        const std::string &password,
        std::string &errorCode,
        callbackFunc &&callbackSuccess
    ) {
        try {
            errorCode.clear();
            driver = sql::mysql::get_driver_instance();
            conn.reset(
                driver->connect("tcp://" + host + ":" + std::to_string(port), UserName, password)
            );
            stmt.reset(conn->createStatement());
            callbackSuccess();
        } catch (const std::exception &e) {
            errorCode = e.what();
            return;
        }
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
    void mysqlCreateTable(
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
            preStmt.reset(conn->prepareStatement(this->cmd));
            preStmt->execute();
            lock.unlock();
            callbackSuccess();
        } catch (const std::exception &e) {
            errorCode = e.what();
        }
    }

    /**
     * @brief 切换操作的数据库
     * @param SchemaName 切换到的数据库名
     * @param errorCode 错误信息
     * @param callbackSuccess 连接成功回调函数
     */
    template <typename callbackFunc>
    void switchDatabase(
        const std::string &SchemaName,
        std::string &errorCode,
        callbackFunc &&callbackSuccess) {
        std::unique_lock lock(sqlMtx);
        try {
            conn->setSchema(SchemaName);
            lock.unlock();
            callbackSuccess();
        } catch (const std::exception &e) {
            errorCode = e.what();
        }
    }

    /**
     * @brief 创建数据库
     * @param SchemaName 要创建的数据库名称
     * @param errorCode 错误信息
     * @param callbackSuccess 连接成功回调函数
     */
    template <typename callbackFunc>
    void createDatabase(
        const std::string &SchemaName,
        std::string &errorCode,
        callbackFunc &&callbackSuccess) {
        std::unique_lock lock(sqlMtx);
        cmd = "CREATE DATABASE IF NOT EXISTS " + SchemaName;
        try {
            stmt->execute(cmd);
            lock.unlock();
            callbackSuccess();
        } catch (const std::exception &e) {
            errorCode = e.what();
        }
    }

    /**
     * @brief 删库
     * @param SchemaName 要删除的数据库名称
     * @warning 跑路啦兄弟，跑路啦！！
     * @param errorCode 错误信息
     * @param callbackSuccess 连接成功回调函数
     */
    template <typename callbackFunc>
    void delDatabase(
        const std::string &SchemaName,
        std::string &errorCode,
        callbackFunc &&callbackSuccess) {
        std::unique_lock lock(sqlMtx);
        cmd = "DROP DATABASE IF EXISTS " + SchemaName;
        try {
            stmt->execute(cmd);
            lock.unlock();
            callbackSuccess();
        } catch (const std::exception &e) {
            errorCode = e.what();
        }
    }

    /**
     * @brief 为表结构增加项目
     * @param tableName 表名
     * @param data 增加内容
     * @param types 增加内容的数据类型
     * @param errorCode 错误信息
     * @param callbackSuccess 连接成功回调函数
     */
    template <typename callbackFunc>
    void addItem(
        const std::string &tableName,
        const item &data,
        const mysqlItemType &types,
        std::string &errorCode,
        callbackFunc &&callbackSuccess) {
        std::unique_lock lock(sqlMtx);
        this->cmd = "insert into " + tableName + "(";
        const auto cnt = data.size();
        for (int i = 0; i < cnt; i++) {
            if (i == cnt - 1) {
                this->cmd += data.at(i).first;
            } else {
                this->cmd += (data.at(i).first + ",");
            }
        }
        this->cmd += ")VALUE(";
        for (int i = 0; i < cnt; i++) {
            if (i == cnt - 1) {
                this->cmd += '?';
            } else {
                this->cmd += "?,";
            }
        }
        this->cmd += ");";

        try {
            preStmt.reset(conn->prepareStatement(this->cmd));

            // 存放 Blob 数据流，需存活到 executeUpdate() 之后
            std::vector<std::unique_ptr<std::istringstream> > blobStreams;

            for (int i = 1; i <= cnt; i++) {
                const std::string &value = data.at(i - 1).second;
                switch (types.at(i - 1)) {
                    case mysqlDataType::BigInt:
                        preStmt->setBigInt(i, value);
                        break;
                    case mysqlDataType::Blob: {
                        // setBlob 惰性读取流，流必须存活到 executeUpdate() 之后
                        blobStreams.emplace_back(std::make_unique<std::istringstream>(value));
                        preStmt->setBlob(i, blobStreams.back().get());
                        break;
                    }
                    case mysqlDataType::Bool:
                        preStmt->setBoolean(i, value == "true" || value == "1");
                        break;
                    case mysqlDataType::DataTime:
                        preStmt->setDateTime(i, value);
                        break;
                    case mysqlDataType::Double:
                        preStmt->setDouble(i, std::stod(value));
                        break;
                    case mysqlDataType::Int32:
                        preStmt->setInt(i, std::stoi(value));
                        break;
                    case mysqlDataType::Int64:
                        preStmt->setInt64(i, std::stoll(value));
                        break;
                    case mysqlDataType::Null:
                        preStmt->setNull(i, sql::DataType::SQLNULL);
                        break;
                    case mysqlDataType::String:
                        preStmt->setString(i, value);
                        break;
                    case mysqlDataType::Uint32:
                        preStmt->setUInt(i, static_cast<uint32_t>(std::stoul(value)));
                        break;
                    case mysqlDataType::Uint64:
                        preStmt->setUInt64(i, std::stoull(value));
                        break;
                }
            }

            preStmt->executeUpdate();
            lock.unlock();
            callbackSuccess();
        } catch (const std::exception &e) {
            errorCode = e.what();
        }
    }

    /**
     * @brief 为表结构删除项目
     * @param tableName 表名
     * @param rule 删除约束
     * @param errorCode 错误信息
     * @param callbackSuccess 连接成功回调函数
     */

    template <typename callbackFunc>
    void delItem(
        const std::string &tableName,
        const itemRule &rule,
        std::string &errorCode,
        callbackFunc &&callbackSuccess
    ) {
        std::unique_lock lock(sqlMtx);
        this->cmd = "delete from " + tableName;
        for (auto i = rule.begin(); i != rule.end(); ++i) {
            cmd += ' ';
            cmd += i == rule.begin() ? "where" : i->link;
            cmd += ' ';
            cmd += i->field + " " += toSql(i->op);
            cmd += '?';
        }

        try {
            const auto cnt = rule.size();
            preStmt.reset(conn->prepareStatement(this->cmd));
            for (int i = 0; i < cnt; i++) {
                preStmt->setString(i + 1, rule[i].value);
            }
            preStmt->execute();
            lock.unlock();
            callbackSuccess();
        } catch (const std::exception &e) {
            errorCode = e.what();
        }
    }

    /**
     * @brief 为表结构更新项目
     * @param tableName 表名
     * @param data 更新内容
     * @param types 更新内容的数据类型
     * @param rule 更新规则
     * @param errorCode 错误信息
     * @param callbackSuccess 连接成功回调函数
     */

    template <typename callbackFunc>
    void updateItem(
        const std::string &tableName,
        const item &data,
        const mysqlItemType &types,
        const itemRule &rule,
        std::string &errorCode,
        callbackFunc &&callbackSuccess
    ) {
        std::unique_lock lock(sqlMtx);
        const auto cnt = data.size();
        this->cmd = "update " + tableName + " set ";
        for (int i = 0; i < cnt; i++) {
            this->cmd += data.at(i).first;
            this->cmd += i == cnt - 1 ? " = ?" : " = ?,";
        }

        for (auto i = rule.begin(); i != rule.end(); ++i) {
            cmd += ' ';
            cmd += (i == rule.begin()) ? "where" : i->link;
            cmd += ' ';
            cmd += i->field + " " += toSql(i->op);
            cmd += '?';
        }

        try {
            preStmt.reset(conn->prepareStatement(this->cmd));
            // 存放 Blob 数据流，需存活到 executeUpdate() 之后
            std::vector<std::unique_ptr<std::istringstream> > blobStreams;
            for (int i = 1; i <= cnt; i++) {
                const std::string &value = data.at(i - 1).second;
                switch (types.at(i - 1)) {
                    case mysqlDataType::BigInt:
                        preStmt->setBigInt(i, value);
                        break;
                    case mysqlDataType::Blob: {
                        // setBlob 惰性读取流，流必须存活到 executeUpdate() 之后
                        blobStreams.emplace_back(std::make_unique<std::istringstream>(value));
                        preStmt->setBlob(i, blobStreams.back().get());
                        break;
                    }
                    case mysqlDataType::Bool:
                        preStmt->setBoolean(i, value == "true" || value == "1");
                        break;
                    case mysqlDataType::DataTime:
                        preStmt->setDateTime(i, value);
                        break;
                    case mysqlDataType::Double:
                        preStmt->setDouble(i, std::stod(value));
                        break;
                    case mysqlDataType::Int32:
                        preStmt->setInt(i, std::stoi(value));
                        break;
                    case mysqlDataType::Int64:
                        preStmt->setInt64(i, std::stoll(value));
                        break;
                    case mysqlDataType::Null:
                        preStmt->setNull(i, sql::DataType::SQLNULL);
                        break;
                    case mysqlDataType::String:
                        preStmt->setString(i, value);
                        break;
                    case mysqlDataType::Uint32:
                        preStmt->setUInt(i, static_cast<uint32_t>(std::stoul(value)));
                        break;
                    case mysqlDataType::Uint64:
                        preStmt->setUInt64(i, std::stoull(value));
                        break;
                }
            }

            for (int j = 0; j < static_cast<int>(rule.size()); j++) {
                preStmt->setString(cnt + 1 + j, rule[j].value);
            }
            preStmt->executeUpdate();
            lock.unlock();
            callbackSuccess();
        } catch (const std::exception &e) {
            errorCode = e.what();
        }
    }

    /**
     * @brief   查找表结构中的内容
     * @param tableName 表名
     * @param data 需查找的表头
     * @param rule 查找约束
     * @param errorCode 错误信息
     * @param callbackSuccess 连接成功回调函数
     * @return json格式的查找结果
     */
    template <typename callbackFunc>
    nlohmann::json searchItem(
        const std::string &tableName,
        const std::vector<std::string> &data,
        const itemRule &rule,
        std::string &errorCode,
        callbackFunc &&callbackSuccess
    ) {
        std::unique_lock lock(sqlMtx);
        this->cmd = "select ";
        if (data.empty()) {
            this->cmd += '*';
        } else {
            for (size_t i = 0; i < data.size(); i++) {
                if (i > 0) {
                    this->cmd += ',';
                }
                this->cmd += data[i];
            }
        }
        this->cmd += " from " + tableName;

        for (auto i = rule.begin(); i != rule.end(); ++i) {
            this->cmd += ' ';
            this->cmd += (i == rule.begin()) ? "where" : i->link;
            this->cmd += ' ';
            this->cmd += i->field + " " += toSql(i->op);
            this->cmd += '?';
        }

        try {
            preStmt.reset(conn->prepareStatement(this->cmd));
            for (int j = 0; j < static_cast<int>(rule.size()); j++) {
                preStmt->setString(j + 1, rule[j].value);
            }
            // MySQL获取资源
            const std::unique_ptr<sql::ResultSet> res(preStmt->executeQuery());
            sql::ResultSetMetaData *meta = res->getMetaData();
            const auto cols = meta->getColumnCount();

            nlohmann::json result = nlohmann::json::object();

            // 初始化每一列为空数组
            for (int c = 1; c <= cols; c++) {
                result[meta->getColumnLabel(c)] = nlohmann::json::array();
            }

            // 遍历结果集，向各列的数组中添加数据
            while (res->next()) {
                for (int c = 1; c <= cols; c++) {
                    result[meta->getColumnLabel(c)].push_back(res->getString(c));
                }
            }
            lock.unlock();
            callbackSuccess();
            return result;
        } catch (const std::exception &e) {
            errorCode = e.what();
        }
        return {};
    }

    // 析构函数
    ~MySQLpp() = default;
};
} // namespace astra_sql