#pragma once

#include <fcntl.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define ADD_SESSION_REC "INSERT INTO sessions (type, activity, duration, timestamp) VALUES (?, ?, ?, ?)"
#define ADD_STAT_REC "INSERT INTO stats (fatigue, stress, interest, timestamp) VALUES (?, ?, ?, ?)"
#define ACTIVITY_FILTER "SELECT * FROM sessions WHERE activity = ? AND timestamp >= ?"
#define CREATE_SESSIONS "CREATE TABLE sessions (id INTEGER PRIMARY KEY, type TEXT, activity TEXT, duration INTEGER, timestamp INTEGER)"
#define CREATE_STATS "CREATE TABLE stats (id INTEGER PRIMARY KEY, fatigue INTEGER, stress INTEGER, interest INTEGER, timestamp INTEGER)"
#define DATABASE_NAME "pomodoro.db"
#define SELECT_ACTIVITY "SELECT DISTINCT activity FROM sessions"
#define SELECT_ALL "SELECT * from stats"
#define SELECT_STATS "SELECT * FROM stats WHERE timestamp >= ?"
#define WORK_FILTER "SELECT * FROM sessions WHERE type = ? AND timestamp >= ?"


void calculate_hours(sqlite3 *db, const char *query, time_t sec, const char *activity);

void create_tables(sqlite3 *db);

void db_error_handler(const char *err_msg, sqlite3 *db);

void export_to_csv(sqlite3 *db, char *file_name);

void fill_session(sqlite3 *db, const char *type, const char *activity, int duration);

void fill_survey(sqlite3 *db);

void get_statistics(time_t sec, sqlite3 *db, const char *query);

void handle_database_error(int result, sqlite3 *db);

int write_lines_to_file(void *file, int argc, char **argv, char **col_names);

int write_lines_to_file_2(void *file, int argc, char **argv, char **col_names);

void exec_query(sqlite3 *db, const char *query, int (*callback)(void *, int, char **, char **), void *param);
