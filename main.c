#include <stdio.h>
#include <stdbool.h>

unsigned int terminalX;
unsigned int terminalY;
unsigned int previousTerminalX;
unsigned int previousTerminalY;

static volatile bool isRunning = true;

#ifdef _WIN32
    #include <windows.h>
#else
    #include <ncurses.h>
    #include <sys/ioctl.h>
    #include <time.h>
    #include <signal.h>
#endif

void mvprint(int x, int y, char ch) {
#ifdef _WIN32
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

int main() {
#ifdef _WIN32
    SetConsoleCtrlHandler(CtrlHandler, TRUE);
#else
    signal(SIGINT, signalHandler);
#endif
    // getTerminalSize() has to be called twice because previousTerminalX and previousTerminalY have to be set
    getTerminalSize();
    getTerminalSize();

    return 0;
}
