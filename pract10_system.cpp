#include <iostream>
#include <Windows.h>
#include <conio.h>
#include <cmath>

using namespace std;

volatile int incrCount = 0;
volatile int decrCount = 0;
volatile int factCount = 0;


HANDLE hIncr, hDecr, hFact, hLoader, hLoader2, hLoader3, hLoader4, hLoader5, hLoader6, hLoader7, hPrinter;

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
    unsigned long long h = 0xcbf29ce484222325ULL;

    while (true) {
        for (int i = 0; i < 100000; i++) {
            h ^= i;
            h *= 0x100000001b3ULL;
        }
    }
}



int main() {
    setlocale(0, "rus");

    hIncr = CreateThread(NULL, 0, incr, NULL, 0, NULL);
    hDecr = CreateThread(NULL, 0, dicr, NULL, 0, NULL);
    hFact = CreateThread(NULL, 0, fact, (LPVOID)100, 0, NULL);
    hLoader = CreateThread(NULL, 0, Loader, NULL, 0, NULL);
    hLoader2 = CreateThread(NULL, 0, Loader, NULL, 0, NULL);
    hLoader3 = CreateThread(NULL, 0, Loader, NULL, 0, NULL);
    hLoader4 = CreateThread(NULL, 0, Loader, NULL, 0, NULL);
    hLoader5 = CreateThread(NULL, 0, Loader, NULL, 0, NULL);
    hLoader6 = CreateThread(NULL, 0, Loader, NULL, 0, NULL);
    hLoader7 = CreateThread(NULL, 0, Loader, NULL, 0, NULL);


    SetThreadPriority(hIncr, THREAD_PRIORITY_ABOVE_NORMAL);
    SetThreadPriority(hDecr, THREAD_PRIORITY_NORMAL);
    SetThreadPriority(hFact, THREAD_PRIORITY_BELOW_NORMAL);

    while (true) {
        system("cls");

        cout << "ID " << GetThreadId(hIncr)
            << " INCR | it/s: " << incrCount
            << " | pr: " << GetThreadPriority(hIncr) << endl;

        cout << "ID " << GetThreadId(hDecr)
            << " DECR | it/s: " << decrCount
            << " | pr: " << GetThreadPriority(hDecr) << endl;

        cout << "ID " << GetThreadId(hFact)
            << " FACT | it/s: " << factCount
            << " | pr: " << GetThreadPriority(hFact) << endl;

        cout << "ID " << GetThreadId(hLoader)
            << " LOAD | "
            << " pr: " << GetThreadPriority(hLoader) << endl;

        cout << "ID " << GetThreadId(hLoader2)
            << " LOAD | "
            << " pr: " << GetThreadPriority(hLoader2) << endl;

        cout << "ID " << GetThreadId(hLoader3)
            << " LOAD | "
            << " pr: " << GetThreadPriority(hLoader3) << endl;

        cout << "ID " << GetThreadId(hLoader4)
            << " LOAD | "
            << " pr: " << GetThreadPriority(hLoader4) << endl;

        cout << "ID " << GetThreadId(hLoader5)
            << " LOAD | "
            << " pr: " << GetThreadPriority(hLoader5) << endl;


        cout << "ID " << GetThreadId(hLoader6)
            << " LOAD | "
            << " pr: " << GetThreadPriority(hLoader6) << endl;

        cout << "ID " << GetThreadId(hLoader7)
            << " LOAD | "
            << " pr: " << GetThreadPriority(hLoader7) << endl;


        cout << "[q/w/e] incr  [a/s/d] decr  [z/x/c] fact" << endl;;
        cout << "[t/y] loader [0] exit" << endl;;

        incrCount = 0;
        decrCount = 0;
        factCount = 0;

        for (int i = 0; i < 10; i++) {
            if (_kbhit()) {
                char c = _getch();
                if (c == '0') return 0;

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
                    SetThreadPriority(hLoader, THREAD_PRIORITY_TIME_CRITICAL);
                    SetThreadPriority(hLoader2, THREAD_PRIORITY_TIME_CRITICAL);
                    SetThreadPriority(hLoader3, THREAD_PRIORITY_TIME_CRITICAL);
                    SetThreadPriority(hLoader4, THREAD_PRIORITY_TIME_CRITICAL);
                    SetThreadPriority(hLoader5, THREAD_PRIORITY_TIME_CRITICAL);
                    SetThreadPriority(hLoader6, THREAD_PRIORITY_TIME_CRITICAL);
                    SetThreadPriority(hLoader7, THREAD_PRIORITY_TIME_CRITICAL);
                    break;

                case 'y':
                    SetThreadPriority(hLoader, THREAD_PRIORITY_NORMAL);
                    SetThreadPriority(hLoader2, THREAD_PRIORITY_NORMAL);
                    SetThreadPriority(hLoader3, THREAD_PRIORITY_NORMAL);
                    SetThreadPriority(hLoader4, THREAD_PRIORITY_NORMAL);
                    SetThreadPriority(hLoader5, THREAD_PRIORITY_NORMAL);
                    SetThreadPriority(hLoader6, THREAD_PRIORITY_NORMAL);
                    SetThreadPriority(hLoader7, THREAD_PRIORITY_NORMAL);
                    break;
                }
            }
            Sleep(100); 
        }
    }
}
