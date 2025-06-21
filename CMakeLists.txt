cmake_minimum_required(VERSION 4.0.2)
project(CPPApiSqlite)

set(CMAKE_CXX_STANDARD 17)

find_package(CURL REQUIRED)
find_package(SQLite3 REQUIRED)

include_directories(${CURL_INCLUDE_DIRS} ${SQLite3_INCLUDE_DIRS} .)

add_executable(CPPApiSqlite
    main.cpp
    api_client.cpp
    database.cpp
)

target_link_libraries(CPPApiSqlite
    ${CURL_LIBRARIES}
    ${SQLite3_LIBRARIES}
)
