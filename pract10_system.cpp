#include <iostream>
#include <Windows.h>
#include <conio.h>
#include <cmath>

using namespace std;

volatile int incrCount = 0;
volatile int decrCount = 0;
volatile int factCount = 0;
volatile bool loaderRunning = false;
volatile bool stopLoader = false;

HANDLE hIncr, hDecr, hFact, hLoader, hPrinter;
int savedPrioIncr, savedPrioDecr, savedPrioFact;

DWORD WINAPI incr(LPVOID) {
    while (true) incrCount++;
}

DWORD WINAPI dicr(LPVOID) {
    while (true) decrCount++;
}

DWORD WINAPI fact(LPVOID param) {
    int n = (int)(intptr_t)param;
    while (true) {
        int r = 1;
        for (int j = 1; j <= n; j++)
            r *= j;
        factCount++;
    }
}

DWORD WINAPI Loader(LPVOID) {
    loaderRunning = true;
    stopLoader = false;
    
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    
    unsigned long long h = 0xcbf29ce484222325ULL;
    while (!stopLoader) {
        for (int i = 0; i < 100000; i++) {
            h ^= i;
            h *= 0x100000001b3ULL;
        }
    }
    
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
    
    loaderRunning = false;
    return 0;
}

DWORD WINAPI Printer(LPVOID) {
    while (true) {
        system("cls");

        cout << "==================== СТАТИСТИКА ====================" << endl;
        cout << "ID " << GetThreadId(hIncr)
            << " | INCR | итераций: " << incrCount
            << " | приоритет: " << GetThreadPriority(hIncr) << endl;

        cout << "ID " << GetThreadId(hDecr)
            << " | DECR | итераций: " << decrCount
            << " | приоритет: " << GetThreadPriority(hDecr) << endl;

        cout << "ID " << GetThreadId(hFact)
            << " | FACT | итераций: " << factCount
            << " | приоритет: " << GetThreadPriority(hFact) << endl;

        if (loaderRunning && hLoader != NULL) {
            cout << "ID " << GetThreadId(hLoader)
                << " | LOADER | активен | приоритет: " << GetThreadPriority(hLoader) << endl;
        }
        else {
            cout << "LOADER | не активен" << endl;
        }

        cout << "====================================================" << endl;
        cout << "управление приоритетами:" << endl;
        cout << "  [q] highest  [w] normal  [e] lowest  - для INCR" << endl;
        cout << "  [a] highest  [s] normal  [d] lowest  - для DECR" << endl;
        cout << "  [z] highest  [x] normal  [c] lowest  - для FACT" << endl;
        cout << endl;
        cout << "  [t] - запустить нагрузчик" << endl;
        cout << "  [y] - остановить нагрузчик" << endl;
        cout << "  [0] - выход" << endl;
        cout << "====================================================" << endl;

        incrCount = 0;
        decrCount = 0;
        factCount = 0;

        Sleep(1000);
    }
    return 0;
}

int main() {
    setlocale(0, "rus");

    hIncr = CreateThread(NULL, 0, incr, NULL, 0, NULL);
    hDecr = CreateThread(NULL, 0, dicr, NULL, 0, NULL);
    hFact = CreateThread(NULL, 0, fact, (LPVOID)100, 0, NULL);
    hPrinter = CreateThread(NULL, 0, Printer, NULL, 0, NULL);

    SetThreadPriority(hIncr, THREAD_PRIORITY_ABOVE_NORMAL);
    SetThreadPriority(hDecr, THREAD_PRIORITY_NORMAL);
    SetThreadPriority(hFact, THREAD_PRIORITY_BELOW_NORMAL);

    hLoader = NULL;

    while (true) {
        if (_kbhit()) {
            char c = _getch();
            
            if (c == '0') {
                if (loaderRunning) {
                    stopLoader = true;
                    WaitForSingleObject(hLoader, 1000);
                    CloseHandle(hLoader);
                }
                return 0;
            }

            switch (c) {
            case 'q': SetThreadPriority(hIncr, THREAD_PRIORITY_HIGHEST); break;
            case 'w': SetThreadPriority(hIncr, THREAD_PRIORITY_NORMAL); break;
            case 'e': SetThreadPriority(hIncr, THREAD_PRIORITY_LOWEST); break;

            case 'a': SetThreadPriority(hDecr, THREAD_PRIORITY_HIGHEST); break;
            case 's': SetThreadPriority(hDecr, THREAD_PRIORITY_NORMAL); break;
            case 'd': SetThreadPriority(hDecr, THREAD_PRIORITY_LOWEST); break;

            case 'z': SetThreadPriority(hFact, THREAD_PRIORITY_HIGHEST); break;
            case 'x': SetThreadPriority(hFact, THREAD_PRIORITY_NORMAL); break;
            case 'c': SetThreadPriority(hFact, THREAD_PRIORITY_LOWEST); break;

            case 't':
                if (!loaderRunning) {
                    savedPrioIncr = GetThreadPriority(hIncr);
                    savedPrioDecr = GetThreadPriority(hDecr);
                    savedPrioFact = GetThreadPriority(hFact);
                    
                    SetThreadPriority(hIncr, THREAD_PRIORITY_IDLE);
                    SetThreadPriority(hDecr, THREAD_PRIORITY_IDLE);
                    SetThreadPriority(hFact, THREAD_PRIORITY_IDLE);
                    
                    hLoader = CreateThread(NULL, 0, Loader, NULL, 0, NULL);
                }
                break;

            case 'y':
                if (loaderRunning) {
                    stopLoader = true;
                    WaitForSingleObject(hLoader, 1000);
                    CloseHandle(hLoader);
                    hLoader = NULL;
                    
                    SetThreadPriority(hIncr, savedPrioIncr);
                    SetThreadPriority(hDecr, savedPrioDecr);
                    SetThreadPriority(hFact, savedPrioFact);
                }
                break;
            }
        }
        Sleep(100);
    }

    return 0;
}
