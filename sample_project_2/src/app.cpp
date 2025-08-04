#include "app.hpp"
#include<iostream>
#include<thread>
#include<chrono>
#include<mutex>
#include<atomic>
std::mutex consolemutex;
std::atomic<bool> userTyping(false);
std::atomic<bool> running(true);

App::App(const std::string& dbpath):database(dbpath){

};




void App::postUserData(int id, const std::string& name, const std::string& email) {
    std::string userJson = "{\"id\":" + std::to_string(id) +
                           ",\"name\":\"" + name +
                           "\",\"email\":\"" + email + "\"}";

    bool dbSuccess = database.insertUsers("[" + userJson + "]");
    bool apiSuccess = apiClient.postUserDataToApi("https://jsonplaceholder.typicode.com/posts", userJson);

    if (dbSuccess && apiSuccess) {
        std::cout << "User posted to DB and API successfully.\n";
    } else if (dbSuccess) {
        std::cerr << "User posted to DB, but API post failed.\n";
    } else {
        std::cerr << "Failed to post user to DB.\n";
    }
}



void App::run(){
    try{
        if(!database.initialize()){
            std::cerr<<"Failed to initialize DB.\n";
            return;
        }

        //apifetching in background thread
        std::thread fetchThread([this]() {
            while (running) {
                if (!userTyping) {
                    std::lock_guard<std::mutex> lock(consolemutex);
                    std::cout << "\n[FETCH] Fetching data from API...\n";

                    std::string jsonData = apiClient.fetchUserData("https://jsonplaceholder.typicode.com/users");

                    if (jsonData.empty()) {
                        std::cerr << "[FETCH] Failed to fetch user data.\n";
                    } else if (database.insertUsers(jsonData)) {
                        std::cout << "[FETCH] Users successfully inserted.\n";
                    } else {
                        std::cerr << "[FETCH] Insert failed.\n";
                    }
                }

                // Smart sleep that checks for shutdown
                for (int i = 0; i < 10 && running; ++i) {
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                }
            }

            
        });



        std::thread postThread([this]() {
            while (true) {
                int id;
                std::string name, email;

                
                {
                    std::lock_guard<std::mutex> lock(consolemutex);
                    std::cout << "\nEnter User ID (or -1 to exit): ";
                }
                userTyping = true;
                std::cin >> id;
                userTyping = false;

                if (std::cin.fail()) {
                    std::cin.clear(); 
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
                    
                    {
                        std::lock_guard<std::mutex> lock(consolemutex);
                        std::cerr << "Invalid input. Please enter a valid number.\n";
                    }
                    
                    continue; // Ask again
                }

                if (id == -1) {
                    {
                        std::lock_guard<std::mutex> lock(consolemutex);
                        std::cout << "Exiting post input...\n";
                    }
                    running = false;
                    break; 
                }
                std::cin.ignore();

                {
                    std::lock_guard<std::mutex> lock(consolemutex);
                    std::cout << "Enter Name: ";
                }
                userTyping = true;
                std::getline(std::cin, name);
                userTyping = false;

                {
                    std::lock_guard<std::mutex> lock(consolemutex);
                    std::cout << "Enter Email: ";
                }
                userTyping = true;
                std::getline(std::cin, email);
                userTyping = false;

                postUserData(id, name, email);
            }
        });

        fetchThread.join();
        postThread.join();
    }
    catch (const std::exception& e) {
        std::cerr << "[EXCEPTION] " << e.what() << '\n';
    }

}