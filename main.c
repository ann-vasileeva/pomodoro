#include "description.h"

int main(int argc, char *argv[]) {
    sqlite3 *db;
    int result = sqlite3_open(DATABASE_NAME, &db);
    handle_database_error(result, db);

    if (argc == 2) {
        if (strcmp(argv[1], "survey") == 0) {
            fill_survey(db);
        } else if (strcmp(argv[1], "help") == 0) {
            help();
        } else {
            handle_errors(bad_input);
        }
    } else if (argc == 3) {
        if (strcmp(argv[1], "export") == 0) {
            export_to_csv(db, argv[2]);
        } else {
            handle_errors(bad_input);
        }
    }
    if (argc == 4) {
        if (strcmp(argv[1], "work") == 0) {
            work_command_handler(db, argv[2], argv[3]);
        } else if (strcmp(argv[1], "statistics") == 0) {
            statistics_command_handler(db, argv[2], argv[3]);
        } else {
            handle_errors(bad_input);
        }
    } else {
        handle_errors(bad_input);
    }
}
