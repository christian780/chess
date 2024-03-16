//#include <fcntl.h>
//#include <stdio.h>
//#include <string.h>
#include <fcntl.h>
#include <iostream>
#include <cstring>
#include <chrono>
#include <QDateTime>



#include "defs.h"

#ifdef _WIN32
#include <Windows.h>
#include <io.h>
#else
#include <sys/time.h>
#include <unistd.h>
#endif

// Rest of your code...

int InputWaiting() {
#ifndef WIN32
    struct timeval tv;
    fd_set readfds;

    FD_ZERO(&readfds);
    FD_SET(fileno(stdin), &readfds);
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    select(16, &readfds, 0, 0, &tv);

    return (FD_ISSET(fileno(stdin), &readfds));
#else
    static int init = 0, pipe;
    static HANDLE inh;
    DWORD dw;

    if (!init) {
        init = 1;
        inh = GetStdHandle(STD_INPUT_HANDLE);
        pipe = !GetConsoleMode(inh, &dw);
        if (!pipe) {
            SetConsoleMode(inh, dw & ~(ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT));
            FlushConsoleInputBuffer(inh);
        }
    }
    if (pipe) {
        if (!PeekNamedPipe(inh, NULL, 0, NULL, &dw, NULL))
            return 1;
        return dw;
    } else {
        GetNumberOfConsoleInputEvents(inh, &dw);
        return dw <= 1 ? 0 : dw;
    }
#endif
}


void ReadInput(S_SEARCHINFO *info) {
    int bytes;
    char input[256] = "";
    char *endc;

    if (InputWaiting()) {
        info->stopped = true;
        do {
            bytes = std::cin.readsome(input, 255); // read input from stdin
        } while (bytes < 0);
        input[bytes] = '\0'; // null-terminate the input string
        endc = strchr(input, '\n');
        if (endc) {
            *endc = 0; // remove newline character
        }

        if (strlen(input) > 0) {
            if (!strncmp(input, "quit", 4)) {
                info->quit = true;
            }
        }
        return;
    }
}




/*class TimeUtility {
public:
    static qint64 GetTimeMs() {
#ifdef Q_OS_WIN
        return QDateTime::currentMSecsSinceEpoch();
#else
        struct timeval t;
        gettimeofday(&t, nullptr);
        return static_cast<qint64>(t.tv_sec) * 1000 + t.tv_usec / 1000;
#endif
    }
};
*/

