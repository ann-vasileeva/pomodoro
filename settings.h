#include "daemon.h"
#include "db_con.h"
#include <string.h>

#define ACTIVITY_NAME_LENGTH 64
#define MAX_INPUT_LENGTH 50
#define MAX_ACTIVITY_LENGTH 256

void change_setting(char *file_path, int new_value) {
    FILE *file = fopen(file_path, "w");
    if (file) {
        fprintf(file, "%d", new_value);
        fclose(file);
    } else {
        handle_error("fopen error");
    }
    printf("Set up!\n");
}

void path_checker(int setup, char *mode) {
    char file_path[ANS_SZ];
    bool correct_path = false;
    while (!correct_path) {
        printf("Please enter the path to your file:\n");
        scanf("%s", file_path);
        FILE *file = fopen(file_path, mode);
        if (file) {
            correct_path = true;
            fclose(file);
        } else {
            printf("Invalid file path or insufficient access rights. Let's try again.\n");
        }
        if (setup) {
            FILE *write_file = fopen("files/path.txt", "w");
            if (write_file != NULL) {
                fprintf(write_file, "%s", file_path);
                fclose(write_file);
            } else {
                handle_error("Failed to set up file path");
            }
        }
    }
    printf("All set up!\n");
}

void change_settings(sqlite3 *db) {
    char ans[ANS_SZ];

    printf("What do you want to change, the sound of the timer or the dial?\n");
    printf("Please select one of these options:\n");
    printf("> sound\n");
    printf("> dial\n");
    scanf("%s", ans);

    if (strcmp(ans, "dial") == 0) {
        printf("Choose your dial\n");
        printf("> classic\n");
        printf("> colorful\n");
        scanf("%s", ans);

        if (strcmp(ans, "classic") == 0) {
            change_setting(dial_setting_path, 0);
        } else if (strcmp(ans, "colorful") == 0) {
            int color;
            printf("Type the number of the color you like\n");
            printf(BLUE "blue - 1\n" RESET);
            printf(GREEN "green - 2\n" RESET);
            printf(RED "red - 3\n" RESET);
            printf(VIOLET "violet - 4\n" RESET);
            scanf("%d", &color);
            change_setting(dial_setting_path, color);
        } else {
            printf("Try again!");
        }
    } else if (strcmp(ans, "sound") == 0) {
        printf("Please select one of these options:\n");
        printf("> To turn on alarm sound, type on\n");
        printf("> To turn off alarm sound, type off\n");
        printf("> To change alarm sound, type change\n");
        scanf("%s", ans);

        if (strcmp(ans, "on") == 0) {
            change_setting(sound_setting_path, 1);
        } else if (strcmp(ans, "off") == 0) {
            change_setting(sound_setting_path, 0);
        } else if (strcmp(ans, "change") == 0) {
            printf("You should provide the path to WAV format files only\n");
            path_checker(1, "r");
        }
    }
}
