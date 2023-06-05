#pragma once

#include <libnotify/notify.h>
#include <signal.h>
#include <stdlib.h>
#include "timer.h"

sig_atomic_t flag = 0;

void handler(int sig) {
    flag = 1;
}

void handle_errors(const char *msg) {
    printf("%s", msg);
    exit(1);
}

void call_daemon(int duration) {
    pid_t pid_id = fork();
    if (pid_id < 0) {
        handle_errors("unsuccessful fork");
    }
    if (pid_id > 0) {
        exit(0);
    } else if (pid_id == 0) {
        pid_t session_id = setsid();
        if (session_id < 0) {
            handle_errors("unsuccessful setsid");
        }
        set_timer(duration, 1);
        _exit(0);
    }

}
