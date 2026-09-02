# MySQL,SQLite,Redis 二次简单封装

## MySQL

- table中数据 的增删改查
- 数据库的增删

## SQLite

- table的增删
- table中数据 的增删改查
- 数据库的增删

## Redis

功能尚未完善

## 构建环境

项目依赖 SQLite、MySQL、Redis 三套库。其中 SQLite 与 Redis（redis-plus-plus + hiredis）由 CMake 自动编译，无需额外安装；**MySQL Connector/C++ 需要单独准备**，定位方式如下（按优先级）：

1. **`-DMySQL_ROOT=<安装根目录>`**
   在 CMake 配置时传入，例如：
   ```bash
   cmake -B build -DMySQL_ROOT=/usr/local/mysql-connector-cpp
   ```

2. **环境变量** `MYSQL_ROOT` 或 `MYSQL_HOME`

3. **自动回退**（未指定以上项时）：
   - **Windows**：使用项目内 `SQL++/lib/mysql_release`（Release）与 `SQL++/lib/mysql_debug`（Debug）。[MySQL库下载](https://dev.mysql.com/downloads/connector/cpp/)
   - **Linux / macOS**：自动通过 `find_path` / `find_library` 探测系统安装的 `mysqlcppconn`（含 Homebrew 的 `/opt/homebrew/opt/mysql-connector-c++`）。

### Windows 说明
- 官方二进制根目录下需存在 `mysql_release` / `mysql_debug`（含 `include/jdbc`、`lib64/vs14` 等）。
- 构建后会自动把 `mysqlcppconn-10-vs14.dll`、`libssl-3-x64.dll`、`libcrypto-3-x64.dll` 复制到输出目录。

### Linux / macOS 说明
- 请安装 MySQL Connector/C++包，或通过 `-DMySQL_ROOT=<path>` 指定安装路径
- 识别库名：`mysqlcppconn`、`mysqlcppconn8`、`mysqlcppconn9`、`mysqlclient`。