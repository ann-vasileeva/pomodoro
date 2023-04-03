#pragma once

#include "daemon.h"
#include "db_con.h"
#include <string.h>

#define ACTIVITY_NAME_LENGTH 64
#define MAX_INPUT_LENGTH 50
#define MAX_ACTIVITY_LENGTH 256
const char *bad_input = "Wrong input\n";

void work_command_handler(sqlite3 *db, char *duration_flag, char *duration_arg) {
    int duration;
    if (strcmp(duration_flag, "-m") == 0) {
        duration = convert_to_sec(0, 0, atoi(duration_arg));
    } else if (strcmp(duration_flag, "-h") == 0) {
        duration = convert_to_sec(0, atoi(duration_arg), 0);
    } else {
        handle_errors(bad_input);
    }

    char rest_ans[MAX_INPUT_LENGTH];
    char activity[ACTIVITY_NAME_LENGTH];
    printf("You can enter an activity for this time here or just press Enter\n");
    fgets(activity, ACTIVITY_NAME_LENGTH, stdin);
    activity[strcspn(activity, "\n")] = '\0';

    printf("Do you want to close your terminal? Answer yes or no\n");
    scanf("%s", rest_ans);
    if (strcmp(rest_ans, "yes") == 0) {
        call_daemon(duration);
    } else {
        set_timer(duration);
    }

    fill_session(db, "work", activity, duration);
    fill_survey(db);

    printf("Do you want to set a rest timer? Answer yes or no\n");
    scanf("%s", rest_ans);
    if (strcmp(rest_ans, "yes") == 0) {
        int rest_duration;
        printf("For how many minutes? Answer with a number\n");
        scanf("%d", &rest_duration);
        set_timer(rest_duration * 60);
    }
}

void statistics_command_handler(sqlite3 *db, char *time_flag, char *time_arg) {
    int time_val;
    if (strcmp(time_flag, "-d") == 0) {
        time_val = convert_to_sec(atoi(time_arg), 0, 0);
    } else if (strcmp(time_flag, "-h") == 0) {
        time_val = convert_to_sec(0, atoi(time_arg), 0);
    } else {
        handle_errors(bad_input);
    }

    char activity[MAX_ACTIVITY_LENGTH];
    printf("If you want statistics for a specific activity only, please type one from this list or just"
           " press Enter. You may choose work\n");
    exec_query(db, SELECT_ACTIVITY, write_lines_to_file, NULL);
    fgets(activity, MAX_ACTIVITY_LENGTH, stdin);
    activity[strcspn(activity, "\n")] = '\0';
    if (strcmp(activity, "") == 0) {
        get_statistics(time_val, db, SELECT_STATS);
    } else {
        calculate_hours(db, ACTIVITY_FILTER, time_val, activity);
    }
}

void help() {
    printf("This is a simple pomodoro timer. Here's a list of the available commands:\n");
    printf("survey: Asks the user to fill out a survey about their session.\n");
    printf("export [filename]: Exports session data to a CSV file with the specified name.\n");
    printf("work [-m/-h] [duration]: Starts a work session for the specified duration in minutes or hours. You can"
           " choose an activity to do. You either set up a timer in terminal or get notified by mail when time runs out\n");
    printf("statistics [-d/-h] [time]: Displays statistics about past sessions for the specified time period in"
           " days or hours. If an activity name is specified, statistics is shown only for that activity.\n");
    printf("help: Displays a list of available commands.\n");
}

