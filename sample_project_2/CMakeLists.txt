cmake_minimum_required(VERSION 3.10)
project(SAMPLE_PROJECT_2)

set(CMAKE_CXX_STANDARD 11)

include_directories(include)

add_executable(ApiSqlite  src/main.cpp src/app.cpp  src/database.cpp  src/api_client.cpp)


find_package(SQLite3 REQUIRED)
find_package(Threads REQUIRED)

target_link_libraries(ApiSqlite  ${CMAKE_THREAD_LIBS_INIT} curl sqlite3)