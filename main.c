#include <stdio.h>

unsigned int terminalX;
unsigned int terminalY;
unsigned int previousTerminalX;
unsigned int previousTerminalY;

#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
#else
    #include <ncurses.h>
    #include <sys/ioctl.h>
#endif

void mvprint(int x, int y, char ch) {
#if defined(_WIN32) || defined(_WIN64)
    COORD coord;

    coord.X = x;
    coord.Y = y;

    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);

    printf("%c", ch);
#else
    mvaddch(y, x, (chtype) ch);
#endif
}

void getTerminalSize() {
    previousTerminalX = terminalX;
    previousTerminalY = terminalY;
#if defined(_WIN32) || defined(_WIN64)
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    
    terminalX = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    terminalY = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
#else
    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);

    terminalX = w.ws_row;
    terminalY = w.ws_col;
#endif
}

int main() {
    // getTerminalSize() has to be called twice because previousTerminalX and previousTerminalY have to be set
    getTerminalSize();
    getTerminalSize();

    return 0;
}
