#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

unsigned int terminalX;
unsigned int terminalY;
unsigned int previousTerminalX;
unsigned int previousTerminalY;

static volatile bool isRunning = true;

char **terminal;

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

    terminalX = w.ws_row;
    terminalY = w.ws_col;
#endif
}

void sleep() { // sleep for 0.05 seconds
#ifdef _WIN32
    Sleep(500);
#else
    nanosleep(50000000);
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
            print(' ');
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
}

int main() {
    unsigned int i, j;

#ifdef _WIN32
    SetConsoleCtrlHandler(CtrlHandler, TRUE);
#else
    signal(SIGINT, signalHandler);
#endif
    // getTerminalSize() has to be called twice because previousTerminalX and previousTerminalY have to be set
    getTerminalSize();
    getTerminalSize();

    srand(time(NULL));

    terminal = (char **) malloc(sizeof(char *) * terminalY);
    for (i = 0; i < terminalY; i++) {
        terminal[i] = (char *) malloc(sizeof(char) * terminalX);
    }

    initializeTerminal();

    while (isRunning) {
        getTerminalSize();

        for (i = terminalY - 1; i >= 1; i--) {
            for (j = 0; j < terminalX; j++) {
                terminal[i][j] = terminal[i - 1][j];
            }
        }

        for (i = 0; i < terminalX; i++) {
            terminal[0][i] = rand() % 95 + 32;
        }

        printTerminal();

        sleep();
    }

    return 0;
}
