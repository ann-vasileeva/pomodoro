cmake_minimum_required(VERSION 3.21)
project(pomodoro C)

set(CMAKE_C_STANDARD 11)

find_package(SQLite3 REQUIRED)
include_directories(${SQLite3_INCLUDE_DIRS})
add_executable(pomodoro main.c db_con.h time.h)
target_link_libraries(pomodoro ${SQLite3_LIBRARIES})