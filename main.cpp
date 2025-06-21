#include "api_client.hpp"
#include "database.hpp"
#include <iostream>

int main() {
    std::string jsonData = fetchUserData();

    if (jsonData.empty()) {
        std::cerr << "Failed to fetch data from API." << std::endl;
        return 1;
    }

    if (!initializeDatabase()) {
        std::cerr << "Database initialization failed." << std::endl;
        return 1;
    }

    if (insertUsers(jsonData)) {
        std::cout << "Users inserted into database!" << std::endl;
    } else {
        std::cerr << "Failed to insert users." << std::endl;
    }

    return 0;
}
