#pragma once

#include <alsa/asoundlib.h>
#include <ncurses.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

enum {
    ANS_SZ = 10000
};

#define RESET   "\033[0m"
#define GREEN   "\033[32m"
#define RED "\033[31m"
#define VIOLET  "\033[35m"
#define BLUE    "\033[34m"

char sound_path[ANS_SZ];
char *sound_setting_path = "files/sound_set.txt";
char *dial_setting_path = "files/dial_set.txt";

int convert_to_sec(int d, int h, int m) {
    int sec = d * 24 * 3600 + h * 3600 + m * 60;
    return sec;
}

void send_notification(const char *title, const char *message) {
    notify_init("Timer");
    NotifyNotification *notification = notify_notification_new(
            "Pomodoro Timer",
            "Time is up!",
            "/home/anna/CLionProjects/pomodoro/files/pomodoro.png"
    );
    notify_notification_show(notification, NULL);

    g_object_unref(G_OBJECT(notification));
    notify_uninit();

}

void handle_error(char *msg) {
    printf("%s\n", msg);
    _exit(0);
}


void play_sound() {
    unsigned int sample_rate = 44100;     // Частота дискретизации
    unsigned int channels = 2;            // Количество каналов
    snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;   // Формат аудио данных (16 бит, little-endian)

    snd_pcm_t *pcm_handle;
    int err = snd_pcm_open(&pcm_handle, "default", SND_PCM_STREAM_PLAYBACK, 0);

    if (err < 0) {
        handle_error("open pcm");
    }
    err = snd_pcm_set_params(pcm_handle, format, SND_PCM_ACCESS_RW_INTERLEAVED, channels, sample_rate, 1,
                             500000);

    if (err < 0) {
        handle_error("set pcm params");
    }

    FILE *r_file = fopen("files/path.txt", "r");
    if (r_file != NULL) {
        fscanf(r_file, "%s", sound_path);
        fclose(r_file);
    } else {
        handle_error("fopen err");
    }

    FILE *wav_file = fopen(sound_path, "rb");
    if (!wav_file) {
        handle_error("fopen err");
    }

    char header[44];
    size_t bytes_read = fread(header, 1, 44, wav_file);
    if (bytes_read != 44) {
        handle_error("header err");
        fclose(wav_file);
    }

    short audio_buffer[44100 * 8];
    size_t data_size = fread(audio_buffer, 2, 44100 * 4, wav_file);
    fclose(wav_file);

    err = snd_pcm_writei(pcm_handle, audio_buffer, data_size);
    if (err < 0) {
        handle_error("pcm write");
    }

    snd_pcm_drain(pcm_handle);
    snd_pcm_close(pcm_handle);
}

void printColored(int colorCode, time_t minutes, time_t seconds) {
    switch (colorCode) {
        case 1:
            printf(BLUE"\rTime left %02ld:%02ld"RESET, minutes, seconds);
            fflush(stdout);
            break;
        case 2:
            printf(GREEN"\rTime left %02ld:%02ld"RESET, minutes, seconds);
            fflush(stdout);
            break;
        case 3:
            printf(RED"\rTime left %02ld:%02ld"RESET, minutes, seconds);
            fflush(stdout);
            break;
        case 4:
            printf(VIOLET"\rTime left %02ld:%02ld"RESET, minutes, seconds);
            fflush(stdout);
            break;
        case 0:
            printf("\rTime left %02ld:%02ld", minutes, seconds);
            fflush(stdout);
            break;
    }
}

void set_timer(int sec, int daemon_calls) {
    time_t cur_time = time(NULL);
    time_t timer_time = cur_time + sec;

    int color;
    FILE *color_file = fopen(dial_setting_path, "r");
    fscanf(color_file, "%d", &color);


    while (cur_time < timer_time) {
        cur_time = time(NULL);
        time_t time_diff = timer_time - cur_time;
        time_t hours = time_diff / 3600;
        time_t minutes = time_diff / 60;
        time_t seconds = time_diff % 60;

        printColored(color, minutes, seconds);
        sleep(1);
    }
    int sound_on;
    FILE *sound_file = fopen(sound_setting_path, "r");
    fscanf(sound_file, "%d", &sound_on);
    if (sound_on) {
        play_sound();
    }
    if (daemon_calls) {
        send_notification("Timer Notification", "Time is up!");
    }

    printf("\nTime is up!\n");
}
