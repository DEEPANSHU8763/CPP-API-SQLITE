#pragma once
#include "api_client.hpp"
#include "database.hpp"

class App {
    public:
        App(const std::string& dbPath = "user.db");
        void postUserData(int id, const std::string& name, const std::string& email);

        void run();

    private:
        ApiClient apiClient;
        Database database;
};