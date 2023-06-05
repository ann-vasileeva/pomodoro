#include "db_con.h"
#include <string.h>


sqlite3_stmt *stmt;

void db_error_handler(const char *err_msg, sqlite3 *db) {
    perror(err_msg);
    sqlite3_close(db);
    exit(1);
}

void handle_database_error(int result, sqlite3 *db) {
    if (result != SQLITE_OK) {
        db_error_handler(sqlite3_errmsg(db), db);
    }
}

int write_lines_to_file(void *file, int argc, char **argv, char **col_names) {
    FILE *csv_output = (FILE *) file;

    for (int i = 0; i < argc; i++) {
        if (argv[i] && argv[i][0] != '\0') {
            fprintf(csv_output, "%s", argv[i]);

            if (i != argc - 1) {
                fprintf(csv_output, ",");
            }
        }
    }

    fprintf(csv_output, "\n");
    return 0;
}

int write_lines_to_file_2(void *file, int argc, char **argv, char **col_names) {
    if (file == NULL) {
        file = stdout;
    }
    for (int i = 0; i < argc; i++) {
        if (argv[i] && argv[i][0] != '\0') {
            fprintf(file, "%s\n", argv[i]);
        }
    }
    return 0;
}

void exec_query(sqlite3 *db, const char *query, int (*callback)(void *, int, char **, char **), void *param) {
    char *err_msg;
    int op_res = sqlite3_exec(db, query, callback, param, &err_msg);

    if (op_res != SQLITE_OK) {
        db_error_handler(err_msg, db);
    }
}

void create_tables(sqlite3 *db) {
    exec_query(db, CREATE_SESSIONS, 0, 0);
    exec_query(db, CREATE_STATS, 0, 0);
}

void fill_survey(sqlite3 *db) {
    stmt = NULL;
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

void get_statistics(time_t sec, sqlite3 *db, const char *query) {
    time_t now = time(NULL);
    time_t max_time_ago = now - sec;
    int op_res = sqlite3_prepare_v2(db, query, -1, &stmt, 0);
    if (op_res != SQLITE_OK) {
        sqlite3_finalize(stmt);
        db_error_handler("couldn't execute insert query\n", db);
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
    printf("Here are your results:\n Your mean fatigue level is %f, stress level is %f, and interest level is %f\n",
           mean_fatigue / rows_cnt,
           mean_stress / rows_cnt, mean_interest / rows_cnt);
}

void fill_session(sqlite3 *db, const char *type, const char *activity, int duration) {
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


void export_to_csv(sqlite3 *db, char *file_name) {
    FILE *csv_output = fopen(file_name, "w+");
    if (!csv_output) {
        db_error_handler("couldn't open file\n", db);
    }
    int op_res = sqlite3_prepare_v2(db, SELECT_ALL, -1, &stmt, NULL);
    if (op_res != SQLITE_OK) {
        db_error_handler("couldn't prepare insert query\n", db);
        return;
    }
    int cols_cnt = sqlite3_column_count(stmt);
    for (int i = 0; i < cols_cnt; ++i) {
        fprintf(csv_output, "%s", sqlite3_column_name(stmt, i));
        if (i != cols_cnt - 1) {
            fprintf(csv_output, ",");
        }
    }
    fprintf(csv_output, "\n");
    op_res = sqlite3_exec(db, SELECT_ALL, write_lines_to_file, csv_output, NULL);
    if (op_res != SQLITE_OK) {
        sqlite3_finalize(stmt);
        db_error_handler("couldn't write to file\n", db);
    }
    if ((op_res = fclose(csv_output)) != 0) {
        sqlite3_finalize(stmt);
        db_error_handler("couldn't write to file\n", db);
    }
    sqlite3_finalize(stmt);
}

void calculate_hours(sqlite3 *db, const char *query, time_t sec, const char *activity) {
    if (strcmp(activity, "work") == 0) {
        query = WORK_FILTER;
    }
    time_t now = time(NULL);
    time_t max_time_ago = now - sec;
    int op_res = sqlite3_prepare_v2(db, query, -1, &stmt, 0);
    if (op_res != SQLITE_OK) {
        sqlite3_finalize(stmt);
        db_error_handler("couldn't execute insert query\n", db);
        return;
    }
    sqlite3_bind_int64(stmt, 2, max_time_ago);
    sqlite3_bind_text(stmt, 1, activity, -1, SQLITE_TRANSIENT);
    int total_time = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        total_time += sqlite3_column_int(stmt, 3);
    }
    sqlite3_finalize(stmt);
    int hours = total_time / 3600;
    int minutes = (total_time % 3600) / 60;
    int seconds = total_time % 60;
    const char *hour_str = hours == 1 ? "hour" : "hours";
    const char *minute_str = minutes == 1 ? "minute" : "minutes";
    const char *seconds_str = seconds == 1 ? "second" : "seconds";
    struct tm *timeinfo = localtime(&max_time_ago);
    char buffer[80];
    strftime(buffer, 80, "%I:%M%p %A, %d %B %Y", timeinfo);
    printf("Here are your results: Total time for %s is %d %s %d %s %d %s since %s \n", activity, hours, hour_str,
           minutes, minute_str, seconds, seconds_str, buffer);
}