#include <ncurses.h>

#define WIDTH 80
#define HEIGHT 24

void draw_bar(int x, int height) {
    int i;
    for (i = 0; i < height; i++) {
        mvprintw(HEIGHT - i, x, "█");
    }
}

int draw_data(const int data[]) {
    int num_bars = 3;

    initscr();
    clear();

    int i;
    int bar_width = WIDTH / (num_bars + 1);

    for (i = 0; i < num_bars; i++) {
        int x = (i + 1) * bar_width;
        int height = data[i];

        draw_bar(x, height);
    }
}