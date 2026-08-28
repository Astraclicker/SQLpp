#pragma once
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <sstream>

#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/connection.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>

#include <SQL.h>
#include <json.hpp>

namespace astra_sql
{

    class MySQLpp
    {
    private:
        // MySQL连接
        std::unique_ptr<sql::Connection> conn;
        // MySQL驱动
        sql::mysql::MySQL_Driver *driver;
        // MySQL执行接口
        std::unique_ptr<sql::PreparedStatement> stmt;
        std::unique_ptr<sql::Statement> statement;
        // 储存MySQL命令
        std::string cmd;

    public:
        /**
         * @brief mysqlpp类构造函数
         * @param host MySQL服务器地址
         * @param port MySQL端口
         * @param UserName MySQL用户名
         * @param password MySQL密码
         */

        MySQLpp(const std::string &host, unsigned int port, const std::string &UserName, const std::string &password);

        // 析构函数
        ~MySQLpp() = default;

        /**
         * @brief 切换操作的数据库
         * @param SchemaName 切换到的数据库名
         */

        MySQLppError switchDatabase(const std::string &SchemaName);

        /**
         * @brief 创建数据库
         * @param SchemaName 要创建的数据库名称
         */
        MySQLppError createDatabase(const std::string &SchemaName);

        /**
         * @brief 删库
         * @param SchemaName 要删除的数据库名称
         * @warning 跑路啦兄弟，跑路啦！！
         */
        MySQLppError delDatabase(const std::string &SchemaName);

        /**
         * @brief 为表结构增加项目
         * @param tableName 表名
         * @param item 增加内容
         * @param itemType 增加内容的数据类型
         */
        MySQLppError addItem(const std::string &tableName, const item &data, const itemType &types);

        /**
         * @brief 为表结构删除项目
         * @param tableName 表名
         * @param itemRule 删除约束
         */
        MySQLppError delItem(const std::string &tableName, const itemRule &rule);

        /**
         * @brief 为表结构更新项目
         * @param tableName 表名
         * @param data 更新内容
         * @param type 更新内容的数据类型
         */
        MySQLppError updateItem(const std::string &tableName, const item &data, const itemType &types, const itemRule &rule);

        /**
         * @brief   查找表结构中的内容
         * @param tableName 表名
         * @param data 需查找的表头
         * @param rule 查找约束
         * @return json格式的查找结果
         */
        nlohmann::json searchItem(const std::string &tableName, const std::vector<std::string> &data, const itemRule &rule);
    };
}
