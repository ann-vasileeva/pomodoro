#pragma once

#include "settings.h"

const char *bad_input = "Wrong input\n";

void printWelcomeMessage() {
    printf(GREEN "-----WELCOME TO POMODORO TIMER-----\n" RESET);
}

void printMenu() {
    printf("Please, select one of these options:\n");
    printf("> To set a work timer, type work\n");
    printf("> To fill a survey, type survey\n");
    printf("> To view statistics, type statistics\n");
    printf("> To export data, type export\n");
    printf("To modify settings, type settings\n");
    printf("> To get help, type help\n");
    printf(GREEN "------------------------------------\n" RESET);
}

void work_command_handler(sqlite3 *db, char *duration_flag, int duration_arg) {
    int duration;

    if (strcmp(duration_flag, "-s") == 0) {
        duration = duration_arg;
    } else if (strcmp(duration_flag, "-m") == 0) {
        duration = convert_to_sec(0, 0, duration_arg);
    } else if (strcmp(duration_flag, "-h") == 0) {
        duration = convert_to_sec(0, duration_arg, 0);
    } else {
        handle_errors(bad_input);
    }

    char rest_ans[MAX_INPUT_LENGTH];
    char activity[ACTIVITY_NAME_LENGTH];

    printf("You can enter an activity for this time here or type work:\n");
    scanf("%s", activity);

    printf("Do you want to close your terminal? Answer yes or no:\n");
    scanf("%s", rest_ans);

    if (strcmp(rest_ans, "yes") == 0) {
        fill_session(db, "work", activity, duration);
        call_daemon(duration);
    } else {
        set_timer(duration, 0);
    }

    fill_session(db, "work", activity, duration);
    fill_survey(db);

    printf("Do you want to set a rest timer? Answer yes or no:\n");
    scanf("%s", rest_ans);

    if (strcmp(rest_ans, "yes") == 0) {
        int rest_duration;
        printf("For how many minutes? Answer with a number:\n");
        scanf("%d", &rest_duration);
        set_timer(rest_duration * 60, 0);
    }
}

void statistics_command_handler(sqlite3 *db, char *time_flag, int time_arg) {
    int time_val;
    if (strcmp(time_flag, "-d") == 0) {
        time_val = convert_to_sec(time_arg, 0, 0);
    } else if (strcmp(time_flag, "-h") == 0) {
        time_val = convert_to_sec(0, time_arg, 0);
    } else {
        handle_errors(bad_input);
    }

    char activity[MAX_ACTIVITY_LENGTH];

    printf("If you want statistics for a specific activity only, please type one from this list or type all. You may choose work:\n");
    exec_query(db, SELECT_ACTIVITY, write_lines_to_file_2, NULL);
    scanf("%s", activity);

    if (strcmp(activity, "all") == 0) {
        get_statistics(time_val, db, SELECT_STATS);
    } else {
        calculate_hours(db, ACTIVITY_FILTER, time_val, activity);
    }
}

void help() {
    printf("This is a simple pomodoro timer. Here's a list of the available commands:\n");
    printf("survey: Asks the user to fill out a survey about their session.\n");
    printf("export [filename]: Exports session data to a CSV file with the specified name.\n");
    printf("work [-s/-m/-h] [duration]: Starts a work session for the specified duration in minutes or hours. You can choose an activity to do. You either set up a timer in terminal or get notified by mail when time runs out.\n");
    printf("statistics [-d/-h] [time]: Displays statistics about past sessions for the specified time period in days or hours. If an activity name is specified, statistics are shown only for that activity.\n");
    printf("help: Displays a list of available commands.\n");
}

void statistics_input(sqlite3 *db) {
    char type[2] = "-";
    int count = 0;

    printf("You'll see statistics for the last time\n");
    printf("Please select the time unit:\n");
    printf("Type d for days or h for hours\n");
    scanf("%s", &type[1]);

    while (strcmp(type, "-h") && strcmp(type, "-d")) {
        printf("Wrong input. Let's try again.\n");
        printf("Type d for days or h for hours\n");
        scanf("%s", &type[1]);
    }

    printf("Next, type the amount of days/hours\n");
    scanf("%d", &count);

    while (count < 0) {
        printf("Wrong input. Let's try again.\n");
        printf("Next, type the amount of days/hours\n");
        scanf("%d", &count);
    }

    statistics_command_handler(db, type, count);
}


void work_input(sqlite3 *db) {
    char type[2] = "-";
    int count = 0;

    printf("Let's set a timer for you! \n");
    printf("Please select the time unit:\n");
    printf("Type m for minutes or h for hours or s for seconds\n");
    scanf("%s", &type[1]);

    while (strcmp(type, "-h") && strcmp(type, "-m") && strcmp(type, "-s")) {
        printf("Wrong input. Let's try again.\n");
        printf("Type m for minutes or h for hours or s for seconds\n");
        scanf("%s", &type[1]);
    }

    printf("Now enter the time in the specified units\n");
    scanf("%d", &count);

    while (count < 0) {
        printf("Wrong input. Let's try again.\n");
        printf("Now enter the time in the specified units\n");
        scanf("%d", &count);
    }

    work_command_handler(db, type, count);
}

void handleUserInput(sqlite3 *db) {
    char answer[ANS_SZ];
    scanf("%s", answer);

    if (strcmp(answer, "survey") == 0) {
        fill_survey(db);
    } else if (strcmp(answer, "help") == 0) {
        help();
    } else if (strcmp(answer, "export") == 0) {
        char file_path[ANS_SZ];
        printf("Enter the file path: ");
        scanf("%s", file_path);
        export_to_csv(db, file_path);
    } else if (strcmp(answer, "statistics") == 0) {
        statistics_input(db);
    } else if (strcmp(answer, "work") == 0) {
        work_input(db);
    } else if (strcmp(answer, "settings") == 0) {
        change_settings(db);
    } else {
        printf("Invalid command\n");
    }
}
