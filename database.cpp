#include "database.hpp"
#include "json.hpp"
#include <sqlite3.h>
#include <iostream>

using json = nlohmann::json;

bool initializeDatabase() {
    sqlite3* db;
    int rc = sqlite3_open("users.db", &db);

    if (rc) {
        std::cerr << "Can't open DB: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    const char* createTableSQL = R"(
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT,
            email TEXT
        );
    )";

    char* errMsg = nullptr;
    rc = sqlite3_exec(db, createTableSQL, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "Create table failed: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        sqlite3_close(db);
        return false;
    }

    sqlite3_close(db);
    return true;
}

bool insertUsers(const std::string& jsonData) {
    sqlite3* db;
    if (sqlite3_open("users.db", &db)) return false;

    json users = json::parse(jsonData);
    sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);

    for (const auto& user : users) {
        std::string name = user["name"];
        std::string email = user["email"];
        std::string sql = "INSERT INTO users (name, email) VALUES (?, ?);";

        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, email.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }

    sqlite3_exec(db, "END TRANSACTION;", nullptr, nullptr, nullptr);
    sqlite3_close(db);
    return true;
}
