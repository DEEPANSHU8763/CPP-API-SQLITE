#pragma once
#include<string>
#include<mutex>

class Database{
public:
    Database(const std::string& dbPath);
    ~Database();

    bool initialize();
    bool insertUsers(const std::string& jsonData);

private:
    std::mutex dbmutex;
    std::string dbPath;
};