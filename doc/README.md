# MySQL Redis SQLite简单二次封装(建议使用msvc进行项目构建)

## 所有数据库封装仅为简单封装想实现更复杂功能可以在链接了本库之后直接include (MySQL,redis++,sqlite3)

### 目前封装的功能

- MySQL
  - 连接数据库
  - 创建数据库
  - 删除数据库
  - table的增删改查
- SQLite
  - 连接数据库
- Redis
  - 暂无

### 所需库(./SQL++/lib)

hiridis,redis-plus-plus,SQLiteCpp均为GitHub项目 \
[mysql_debug与mysql_release下载](https://dev.mysql.com/downloads/connector/cpp/)
