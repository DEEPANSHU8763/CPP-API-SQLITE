#include "database.hpp"
#include<sqlite3.h>
#include<iostream>
#include "json.hpp"  // for nlohmann::json
using json = nlohmann::json;

Database::Database(const std::string& dbPath):dbPath(dbPath){}

Database::~Database() {}





bool Database::initialize(){
    sqlite3* db;
    int result = sqlite3_open(dbPath.c_str(),&db);
    if (result) {
    std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
    return false;
}


    const char* sql = "CREATE TABLE IF NOT EXISTS users ("
    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "name TEXT NOT NULL,"
    "email TEXT NOT NULL);";

    result = sqlite3_exec(db,sql,nullptr,nullptr,nullptr);
    if(result != SQLITE_OK){
        std::cerr<<"FAILED TO CREATE TABLE.\n";
        sqlite3_close(db);
        return false;
    }

    sqlite3_close(db);
    return true;



};  





bool Database::insertUsers(const std::string& jsonData){
    // std::lock_guard<std::mutex> lock(dbmutex);
    sqlite3* db;
    int result = sqlite3_open(dbPath.c_str(),&db);
    if(result){
        std::cerr<<"can't open DB"<<sqlite3_errmsg(db)<<std::endl;
        return false;
    }

    try {
        json users = json::parse(jsonData);

        if(!users.is_array()){
            std::cerr<<"expected json array\n";
            sqlite3_close(db);
            return false;
        }

        const char* insertsql = "INSERT OR REPLACE INTO users (id, name, email) VALUES (?,?,?);";
        sqlite3_exec(db, "BEGIN TRANSACTION;",nullptr,nullptr,nullptr);

        for(const auto& user: users){
            
            if(!user.contains("id")||!user.contains("name")||!user.contains("email")) {
                std::cerr<<"skip incomplete user\n";
                continue;
            }

            int id = user["id"];
            std::string name = user["name"];
            std::string email = user["email"];


            sqlite3_stmt* stmt;
            result = sqlite3_prepare_v2(db, insertsql, -1, &stmt, nullptr);
            
            
            if (result != SQLITE_OK) {
                std::cerr << " Failed to prepare statement: " << sqlite3_errmsg(db) << "\n";
                continue;
            }

            


            sqlite3_bind_int(stmt, 1, id);
            sqlite3_bind_text(stmt, 2, name.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 3, email.c_str(), -1, SQLITE_STATIC);

            result = sqlite3_step(stmt);
            if(result != SQLITE_DONE){
                std::cerr<<"insert failed:"<<sqlite3_errmsg(db)<<"\n";

            }
            sqlite3_finalize(stmt);
            
    
        }

        sqlite3_exec(db, "END TRANSACTION;",nullptr,nullptr,nullptr);
        sqlite3_close(db);
        return true;
    }
    catch(const std::exception& e){
        std::cerr<<"error"<<e.what()<<"\n";
        sqlite3_close(db);
        return false;
    }


};
