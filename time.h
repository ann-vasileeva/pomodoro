#pragma once
#include <time.h>
#include <stdio.h>
#include <unistd.h>

int convert_to_sec(int d, int h, int m) {
    int sec = d * 24 * 3600 + h * 3600 + m * 60;
    return sec;
}

void set_timer(int sec) {
    time_t cur_time = time(NULL);
    time_t timer_time = cur_time + sec;
    while (cur_time < timer_time) {
        cur_time = time(NULL);
        time_t time_diff = timer_time - cur_time;
        time_t minutes = time_diff / 60;
        time_t seconds = time_diff % 60;
        printf("\rTime left %ld:%ld", minutes, seconds);
        fflush(stdout);
        sleep(1);
    }
    printf("\nTime is up!\n");
}