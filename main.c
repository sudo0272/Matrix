#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

unsigned int terminalX;
unsigned int terminalY;
unsigned int previousTerminalX;
unsigned int previousTerminalY;

static volatile bool isRunning = true;

char **terminal;

unsigned int *printCount;
unsigned int *delayCount;

#ifdef _WIN32
    #include <windows.h>
#else
    #include <ncurses.h>
    #include <sys/ioctl.h>
    #include <time.h>
    #include <signal.h>
#endif

void moveCursor(int x, int y) {
#ifdef _WIN32
    COORD coord;

    coord.X = x;
    coord.Y = y;

    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
#else
    move(y, x);
#endif
}

void print(char ch) {
#ifdef _WIN32
    printf("%c", ch);
#else
    addch(ch);
#endif
}

void getTerminalSize() {
    previousTerminalX = terminalX;
    previousTerminalY = terminalY;
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    
    terminalX = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    terminalY = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
#else
    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);

    terminalX = w.ws_col;
    terminalY = w.ws_row;
#endif
}

void sleep() { // sleep for 0.05 seconds
#ifdef _WIN32
    Sleep(50);
#else
    usleep(50000);
#endif
}

#ifdef _WIN32
    void WINAPI CtrlHandler(DWORD fdwCtrlType) {
        if (fdwCtrlType == CTRL_C_EVENT) {
            isRunning = false;
        }
    }
#else
    static void signalHandler(int t) {
        (void) t;

        isRunning = false;
    }
#endif

void initializeTerminal() {
    unsigned int i, j;

    moveCursor(0, 0);

    for (i = 0; i < terminalY; i++) {
        for (j = 0; j < terminalX; j++) {
            terminal[i][j] = ' ';
        }
    }
}

void printTerminal() {
    unsigned int i, j;

    moveCursor(0, 0);

    for (i = 0; i < terminalY; i++) {
        for (j = 0; j < terminalX; j++) {
            print(terminal[i][j]);
        }
    }

#ifndef _WIN32
    refresh();
#endif
}

int main() {
    unsigned int i, j;

#ifdef _WIN32
    SetConsoleCtrlHandler(CtrlHandler, TRUE);
#else
    signal(SIGINT, signalHandler);

    initscr();
#endif
    // getTerminalSize() has to be called twice because previousTerminalX and previousTerminalY have to be set
    getTerminalSize();
    getTerminalSize();

    srand(time(NULL));

    terminal = (char **) malloc(sizeof(char *) * terminalY);
    for (i = 0; i < terminalY; i++) {
        terminal[i] = (char *) malloc(sizeof(char) * terminalX);
    }

    printCount = (unsigned int *) malloc(sizeof(unsigned int) * terminalX);
    delayCount = (unsigned int *) malloc(sizeof(unsigned int) * terminalX);

    for (i = 0; i < terminalX; i += 2) {
        printCount[i] = rand() % 11;
        delayCount[i] = 0;
    }

    initializeTerminal();

    while (isRunning) {
        getTerminalSize();

        for (i = terminalY - 1; i >= 1; i--) {
            for (j = 0; j < terminalX; j += 2) {
                terminal[i][j] = terminal[i - 1][j];
            }
        }

        for (i = 0; i < terminalX; i += 2) {
            if (printCount[i] < 10) {
                terminal[0][i] = rand() % 94 + 33;
                printCount[i]++;
            } else {
                delayCount[i]++;

                if (delayCount[i] == 6) {
                    printCount[i] = rand() % 11;
                    delayCount[i] = 0;
                }

                terminal[0][i] = ' ';
            }
        }

        printTerminal();

        sleep();
    }

#ifndef _WIN32
    endwin();
#endif

    return 0;
}
