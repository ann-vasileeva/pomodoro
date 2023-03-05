#include "db_con.h"
#include "time.h"

int main(int argc, char *argv[]) {
    sqlite3 *db;
    int op_res = sqlite3_open("pomodoro.db", &db);
    if (op_res != SQLITE_OK) {
        db_error_handler("can't open database\n", db);
    }

    if (argc == 2) {
        if (strcmp(argv[1], "survey") == 0) {
            fill_survey(db);
        } else {
            printf("Wrong input format\n");
            exit(1);
        }
    }
    if (argc == 4) {
        if (strcmp(argv[1], "work") == 0) {
            char activity[64];
            int duration;
            char rest_ans[3];
            if (strcmp(argv[2], "-m") == 0) {
                duration = convert_to_sec(0, 0, atoi(argv[3]));
            } else if (strcmp(argv[2], "-h") == 0) {
                duration = convert_to_sec(0, atoi(argv[3]), 0);
            } else {
                printf("Wrong input\n");
                exit(1);
            }
            printf("You can enter an activity for this time here or just press Enter\n");
            fgets(activity, 64, stdin);
            activity[strcspn(activity, "\n")] = '\0';

            set_timer(duration);
            fill_session(db, argv[1], activity, duration);

            fill_survey(db);

            printf("Do you want to set a rest timer? Answer yes or no\n");
            scanf("%s", rest_ans);
            if (strcmp(rest_ans, "yes") == 0) {
                int rest_duration;
                printf("For how many minutes? Answer with a number\n");
                scanf("%d", &rest_duration);
                set_timer(rest_duration * 60);
            } else {
                return 0;
            }
        } else if (strcmp(argv[1], "statistics") == 0) {
            int time_val;
            if (strcmp(argv[2], "-d") == 0) {
                time_val = convert_to_sec(atoi(argv[3]), 0, 0);
            } else if (strcmp(argv[2], "-h") == 0) {
                time_val = convert_to_sec(0, atoi(argv[3]), 0);
            } else {
                printf("Wrong input\n");
                exit(1);
            }
            get_statistics(time_val, db);
        } else {
            printf("Wrong input format\n");
            exit(1);
        }
    }

    sqlite3_close(db);
    return 0;
}