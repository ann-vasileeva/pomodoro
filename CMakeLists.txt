cmake_minimum_required(VERSION 3.21)
project(pomodoro C)


set(CMAKE_C_STANDARD 11, .clang-format)
find_package(SQLite3 REQUIRED)
find_package(Gnuplot REQUIRED)
include_directories(${GNUPLOT_INCLUDE_DIRS})
add_executable(pomodoro main.c db_con.h timer.h description.h db_con.c daemon.h)
target_link_libraries(pomodoro ${GNUPLOT_LIBRARIES})
include_directories(${SQLite3_INCLUDE_DIRS})

target_link_libraries(pomodoro ${SQLite3_LIBRARIES})