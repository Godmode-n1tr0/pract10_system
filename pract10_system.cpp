#include <iostream>
#include <Windows.h>
#include <conio.h>
using namespace std;

volatile long long cnt[3] = { 0, 0, 0 };
volatile bool run[3] = { true, true, true };
volatile bool stressRunning = false;

HANDLE th[3] = { NULL, NULL, NULL };
DWORD id[3] = { 0, 0, 0 };

const char* PriorityToString(int prio) {
    switch (prio) {
    case -15: return "фоновый";
    case -2:  return "низкий";
    case -1:  return "ниже нормального";
    case 0:   return "нормальный";
    case 1:   return "выше нормального";
    case 2:   return "высокий";
    case 15:  return "максимальный";
    default:  return "нормальный";
    }
}

DWORD WINAPI inc(LPVOID p) {
    while (run[0]) {
        cnt[0]++;
    }
    return 0;
}

DWORD WINAPI fib(LPVOID p) {
    long long a, b, c;
    while (run[1]) {
        a = 0; b = 1;
        for (int i = 0; i < 20; i++) {
            c = a + b;
            a = b;
            b = c;
        }
        cnt[1]++;
    }
    return 0;
}

DWORD WINAPI fact(LPVOID p) {
    long long f;
    while (run[2]) {
        f = 1;
        for (int i = 1; i <= 12; i++) {
            f *= i;
        }
        cnt[2]++;
    }
    return 0;
}

DWORD WINAPI StressThreadProc(LPVOID) {
    stressRunning = true;
    while (stressRunning) {
        int x = 0;
        x++;
    }
    return 0;
}

DWORD WINAPI logger(LPVOID p) {
    const char* names[] = { "инкремент", "фибоначчи", "факториал" };

    while (true) {
        Sleep(1000);

        for (int i = 0; i < 3; i++) {
            int pr = GetThreadPriority(th[i]);
            cout << "поток " << names[i] << " | ид: " << id[i]
                << " | итераций: " << cnt[i]
                << " | приоритет: " << PriorityToString(pr) << " (" << pr << ")" << endl;
        }
        cout << "-----------------------------" << endl;
    }
    return 0;
}

void set_pr(int tid, int val) {
    if (tid < 0 || tid > 2) return;

    int pr = 0;
    switch (val) {
    case 1: pr = -15; break;
    case 2: pr = -2; break;
    case 3: pr = -1; break;
    case 4: pr = 0; break;
    case 5: pr = 1; break;
    case 6: pr = 2; break;
    case 7: pr = 15; break;
    default: return;
    }

    SetThreadPriority(th[tid], pr);
    int confirmed = GetThreadPriority(th[tid]);
    cout << "приоритет потока " << tid << " изменен на " << PriorityToString(confirmed) << " (" << confirmed << ")" << endl;
}

int main() {
    setlocale(0, "rus");

    cout << "потоки будут запущены со следующими приоритетами:" << endl;
    cout << "поток 1 - инкремент: низкий (-2)" << endl;
    cout << "поток 2 - фибоначчи: нормальный (0)" << endl;
    cout << "поток 3 - факториал: высокий (2)" << endl;
    cout << endl << "нажмите любую клавишу для запуска потоков..." << endl;
    _getch();

    th[0] = CreateThread(NULL, 0, inc, (void*)0, 0, &id[0]);
    th[1] = CreateThread(NULL, 0, fib, (void*)1, 0, &id[1]);
    th[2] = CreateThread(NULL, 0, fact, (void*)2, 0, &id[2]);

    SetThreadPriority(th[0], -2);
    SetThreadPriority(th[1], 0);
    SetThreadPriority(th[2], 2);

    HANDLE hlog = CreateThread(NULL, 0, logger, NULL, 0, NULL);

    HANDLE stressThreadHandle = nullptr;
    bool quit = false;

    cout << "-----------------------------" << endl;
    cout << "команды:" << endl;
    cout << "  1 - изменить приоритет инкремента" << endl;
    cout << "  2 - изменить приоритет фибоначчи" << endl;
    cout << "  3 - изменить приоритет факториала" << endl;
    cout << "  4 - запустить/остановить нагрузчик" << endl;
    cout << "  5 - выход" << endl;
    cout << "-----------------------------" << endl;

    while (!quit) {
        if (_kbhit()) {
            char ch = _getch();
            
            if (ch == '1' || ch == '2' || ch == '3') {
                int idx = ch - '1';
                cout << endl << "новый приоритет для потока " << (idx + 1) << ":" << endl;
                cout << "1: фоновый (-15)" << endl;
                cout << "2: низкий (-2)" << endl;
                cout << "3: ниже нормального (-1)" << endl;
                cout << "4: нормальный (0)" << endl;
                cout << "5: выше нормального (1)" << endl;
                cout << "6: высокий (2)" << endl;
                cout << "7: максимальный (15)" << endl;
                cout << "выбор: ";
                
                char pch = _getch();
                int prio = 0;
                if (pch == '1') prio = -15;
                else if (pch == '2') prio = -2;
                else if (pch == '3') prio = -1;
                else if (pch == '4') prio = 0;
                else if (pch == '5') prio = 1;
                else if (pch == '6') prio = 2;
                else if (pch == '7') prio = 15;
                else prio = 0;

                SetThreadPriority(th[idx], prio);
                int confirmed = GetThreadPriority(th[idx]);
                cout << endl << "установлен: " << PriorityToString(confirmed) << " (" << confirmed << ")" << endl;
            }
            else if (ch == '4') {
                if (stressRunning) {
                    stressRunning = false;
                    WaitForSingleObject(stressThreadHandle, INFINITE);
                    CloseHandle(stressThreadHandle);
                    stressThreadHandle = nullptr;
                    cout << endl << "нагрузчик остановлен" << endl;
                }
                else {
                    stressThreadHandle = CreateThread(NULL, 0, StressThreadProc, NULL, 0, NULL);
                    SetThreadPriority(stressThreadHandle, 15);
                    cout << endl << "нагрузчик запущен с приоритетом максимальный (15)" << endl;
                }
            }
            else if (ch == '5') {
                quit = true;
            }
        }
        Sleep(100);
    }

    if (stressThreadHandle) {
        stressRunning = false;
        WaitForSingleObject(stressThreadHandle, INFINITE);
        CloseHandle(stressThreadHandle);
    }

    for (int i = 0; i < 3; i++) {
        run[i] = false;
    }

    WaitForSingleObject(th[0], 1000);
    WaitForSingleObject(th[1], 1000);
    WaitForSingleObject(th[2], 1000);

    CloseHandle(th[0]);
    CloseHandle(th[1]);
    CloseHandle(th[2]);
    CloseHandle(hlog);

    cout << "программа завершена" << endl;

    return 0;
}
