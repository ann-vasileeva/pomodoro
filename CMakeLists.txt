cmake_minimum_required(VERSION 3.21)
project(pomodoro C)

set(CMAKE_C_STANDARD 11, .clang-format)
find_package(SQLite3 REQUIRED)
find_package(Gnuplot REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB REQUIRED glib-2.0)
pkg_check_modules(NOTIFY REQUIRED libnotify)
include_directories(${GNUPLOT_INCLUDE_DIRS})
include_directories(${GLIB_INCLUDE_DIRS} ${NOTIFY_INCLUDE_DIRS})
link_directories(${GLIB_LIBRARY_DIRS} ${NOTIFY_LIBRARY_DIRS})
add_executable(pomodoro main.c db_con.h timer.h description.h db_con.c daemon.h plots.h settings.h)
target_link_libraries(pomodoro ${GNUPLOT_LIBRARIES})
target_link_libraries(pomodoro ncurses)
find_library(ALSA_LIBRARY asound)
target_link_libraries(pomodoro ${ALSA_LIBRARY})
include_directories(${SQLite3_INCLUDE_DIRS})

target_link_libraries(pomodoro ${SQLite3_LIBRARIES})
target_link_libraries(pomodoro ${GLIB_LIBRARIES} ${NOTIFY_LIBRARIES})