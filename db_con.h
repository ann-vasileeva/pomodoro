#pragma once
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <time.h>
#include <unistd.h>

#define CREATE_SESSIONS "CREATE TABLE sessions (id INTEGER PRIMARY KEY, type TEXT, activity TEXT, duration INTEGER, timestamp INTEGER)"
#define CREATE_STATS "CREATE TABLE stats (id INTEGER PRIMARY KEY, fatigue INTEGER, stress INTEGER, interest INTEGER, timestamp INTEGER)"
#define ADD_STAT_REC "INSERT INTO stats (fatigue, stress, interest, timestamp) VALUES (?, ?, ?, ?)"
#define ADD_SESSION_REC "INSERT INTO sessions (type, activity, duration, timestamp) VALUES (?, ?, ?, ?)"
#define SELECT_STATS "SELECT * FROM stats WHERE timestamp >= ?"

void db_error_handler(char *err_msg, sqlite3 *db) {
    perror(err_msg);
    sqlite3_close(db);
    exit(1);
}

void exec_query(sqlite3 *db, const char *query) {
    char *err_msg;
    int op_res = sqlite3_exec(db, query, 0, 0, &err_msg);
    if (op_res != SQLITE_OK) {
        db_error_handler(err_msg, db);
    }
}

void create_tables(sqlite3 *db) {
    exec_query(db, CREATE_SESSIONS);
    exec_query(db, CREATE_STATS);
}

void fill_survey(sqlite3 *db) {
    sqlite3_stmt *stmt;
    int fatigue = 0, stress = 0, interest = 0;
    printf("This survey will help you track your well-being\n");

    while (fatigue <= 0 || fatigue > 10) {
        printf("How do you feel right now on a scale of 1 to 10, where 1 means fully rested and 10 means very tired?\n");
        scanf("%d", &fatigue);
        if (fatigue <= 0 || fatigue > 10) {
            printf("Invalid input, please enter a number between 1 and 10.\n");
        }
    }

    while (stress <= 0 || stress > 10) {
        printf("On a scale of 1 to 10, how stressed do you feel right now, where 1 means no stress and 10 means extremely stressed\n");
        scanf("%d", &stress);
        if (stress <= 0 || stress > 10) {
            printf("Invalid input, please enter a number between 1 and 10.\n");
        }
    }

    while (interest <= 0 || interest > 10) {
        printf("On a scale of 1 to 10, how interested do you feel in your current activity, where 1 means no interest and 10 means very interested and enjoying it?\n");
        scanf("%d", &interest);
        if (interest <= 0 || interest > 10) {
            printf("Invalid input, please enter a number between 1 and 10.\n");
        }
    }

    int op_res = sqlite3_prepare_v2(db, ADD_STAT_REC, -1, &stmt, 0);
    if (op_res != SQLITE_OK) {
        db_error_handler("couldn't prepare insert query\n", db);
    }
    time_t now = time(NULL);
    sqlite3_bind_int(stmt, 1, fatigue);
    sqlite3_bind_int(stmt, 2, stress);
    sqlite3_bind_int(stmt, 3, interest);
    sqlite3_bind_int64(stmt, 4, now);
    op_res = sqlite3_step(stmt);
    if (op_res != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        db_error_handler("couldn't execute insert query\n", db);
    }
    sqlite3_finalize(stmt);
}

void get_statistics(time_t sec, sqlite3 *db) {
    sqlite3_stmt *stmt;
    time_t now = time(NULL);
    time_t max_time_ago = now - sec;
    int op_res = sqlite3_prepare_v2(db, SELECT_STATS, -1, &stmt, 0);
    if (op_res != SQLITE_OK) {
        sqlite3_finalize(stmt);
    }
    sqlite3_bind_int64(stmt, 1, max_time_ago);
    double mean_fatigue = 0.0;
    double mean_stress = 0;
    double mean_interest = 0;
    int rows_cnt = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        ++rows_cnt;
        mean_fatigue += sqlite3_column_int(stmt, 1);
        mean_stress += sqlite3_column_int(stmt, 2);
        mean_interest += sqlite3_column_int(stmt, 3);
    }
    sqlite3_finalize(stmt);
    if (!rows_cnt) {
        printf("Oops, there is no data for this period\n");
        return;
    }
    printf("Here are your results: Your mean fatigue level is %f, stress level is %f, and interest level is %f\n",
           mean_fatigue / rows_cnt,
           mean_stress / rows_cnt, mean_interest / rows_cnt);
}

void fill_session(sqlite3 *db, const char *type, const char *activity, int duration) {
    sqlite3_stmt *stmt;

    int op_res = sqlite3_prepare_v2(db, ADD_SESSION_REC, -1, &stmt, 0);
    if (op_res != SQLITE_OK) {
        db_error_handler("couldn't prepare insert query\n", db);
        return;
    }

    time_t now = time(NULL);
    sqlite3_bind_text(stmt, 1, type, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, activity, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, duration);
    sqlite3_bind_int64(stmt, 4, now);

    op_res = sqlite3_step(stmt);
    if (op_res != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        db_error_handler("couldn't execute insert query\n", db);
    }

    sqlite3_clear_bindings(stmt);
    sqlite3_reset(stmt);
    sqlite3_finalize(stmt);
}