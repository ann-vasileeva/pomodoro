#include "description.h"
#include <sqlite3.h>

int main(int argc, char *argv[]) {
    sqlite3 *db;
    int result = sqlite3_open(DATABASE_NAME, &db);
    handle_database_error(result, db);

    if (argc == 1) {
        printWelcomeMessage();
        printMenu();
        handleUserInput(db);
    } else if (argc == 2) {
        if (strcmp(argv[1], "survey") == 0) {
            fill_survey(db);
        } else if (strcmp(argv[1], "help") == 0) {
            help();
        } else {
            printf("Invalid command\n");
        }
    } else if (argc == 3) {
        if (strcmp(argv[1], "export") == 0) {
            export_to_csv(db, argv[2]);
        } else {
            printf("Invalid command\n");
        }
    } else if (argc == 4) {
        int time = atoi(argv[3]);
        if (strcmp(argv[1], "work") == 0) {
            work_command_handler(db, argv[2], time);
        } else if (strcmp(argv[1], "statistics") == 0) {
            statistics_command_handler(db, argv[2], time);
        } else {
            printf("Invalid command\n");
        }
    } else {
        printf("Invalid command\n");
    }
    sqlite3_close(db);
    return 0;
}