#include <windows.h>
#include <iostream>
#include <cmath>
using namespace std;

volatile long long cnt[3] = { 0, 0, 0 };
volatile bool run[3] = { true, true, true };
volatile bool load_run = false;
volatile bool load_stop = false;

HANDLE th[3] = { NULL, NULL, NULL };
DWORD id[3] = { 0, 0, 0 };
int saved_prio[3] = { 0, 0, 0 };

DWORD WINAPI inc(LPVOID p) {
    int n = (int)p;
    while (run[n]) {
        cnt[n]++;
    }
    return 0;
}

DWORD WINAPI fib(LPVOID p) {
    int n = (int)p;
    long long a, b, c;
    while (run[n]) {
        a = 0; b = 1;
        for (int i = 0; i < 20; i++) {
            c = a + b;
            a = b;
            b = c;
        }
        cnt[n]++;
    }
    return 0;
}

DWORD WINAPI fact(LPVOID p) {
    int n = (int)p;
    long long f;
    while (run[n]) {
        f = 1;
        for (int i = 1; i <= 12; i++) {
            f *= i;
        }
        cnt[n]++;
    }
    return 0;
}

DWORD WINAPI loader(LPVOID p) {
    cout << "нагрузчик запущен с приоритетом максимальный" << endl;
    
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    
    int counter = 0;
    int max = 1000;
    
    for (int t = 0; t < 15000; t++) {
        counter = 0;
        while (counter < max) {
            counter++;
        }
    }
    
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
    
    cout << "нагрузчик завершил работу, приоритет нормальный" << endl;
    
    load_run = false;
    return 0;
}

DWORD WINAPI logger(LPVOID p) {
    const char* names[] = { "инкремент", "фибоначчи", "факториал" };

    while (true) {
        Sleep(1000);

        for (int i = 0; i < 3; i++) {
            int pr = GetThreadPriority(th[i]);
            const char* pr_str = "нормальный";

            if (pr == THREAD_PRIORITY_IDLE) pr_str = "фоновый";
            else if (pr == THREAD_PRIORITY_LOWEST) pr_str = "низкий";
            else if (pr == THREAD_PRIORITY_BELOW_NORMAL) pr_str = "ниже нормального";
            else if (pr == THREAD_PRIORITY_NORMAL) pr_str = "нормальный";
            else if (pr == THREAD_PRIORITY_ABOVE_NORMAL) pr_str = "выше нормального";
            else if (pr == THREAD_PRIORITY_HIGHEST) pr_str = "высокий";
            else if (pr == THREAD_PRIORITY_TIME_CRITICAL) pr_str = "максимальный";

            cout << "поток " << names[i] << " | ид: " << id[i]
                << " | итераций: " << cnt[i]
                << " | приоритет: " << pr_str << endl;
        }
        cout << "-----------------------------" << endl;
    }
    return 0;
}

void set_pr(int tid, int val) {
    if (tid < 0 || tid > 2) return;

    int pr = THREAD_PRIORITY_NORMAL;
    switch (val) {
    case 1: pr = THREAD_PRIORITY_IDLE; break;
    case 2: pr = THREAD_PRIORITY_LOWEST; break;
    case 3: pr = THREAD_PRIORITY_BELOW_NORMAL; break;
    case 4: pr = THREAD_PRIORITY_NORMAL; break;
    case 5: pr = THREAD_PRIORITY_ABOVE_NORMAL; break;
    case 6: pr = THREAD_PRIORITY_HIGHEST; break;
    default: return;
    }

    SetThreadPriority(th[tid], pr);
    cout << "приоритет потока " << tid << " изменен" << endl;
}

int main() {
    setlocale(0, "rus");

    th[0] = CreateThread(NULL, 0, inc, (void*)0, 0, &id[0]);
    th[1] = CreateThread(NULL, 0, fib, (void*)1, 0, &id[1]);
    th[2] = CreateThread(NULL, 0, fact, (void*)2, 0, &id[2]);

    SetThreadPriority(th[0], THREAD_PRIORITY_IDLE);
    SetThreadPriority(th[1], THREAD_PRIORITY_BELOW_NORMAL);
    SetThreadPriority(th[2], THREAD_PRIORITY_NORMAL);

    HANDLE hlog = CreateThread(NULL, 0, logger, NULL, 0, NULL);

    cout << "программа запущена" << endl;
    cout << "-----------------------------" << endl;
    cout << "1 - фоновый" << endl;
    cout << "2 - низкий" << endl;
    cout << "3 - ниже нормального" << endl;
    cout << "4 - нормальный" << endl;
    cout << "5 - выше нормального" << endl;
    cout << "6 - высокий" << endl;
    cout << "-----------------------------" << endl;
    cout << "команды:" << endl;
    cout << "  номер_потока приоритет" << endl;
    cout << "  пример: 0 4" << endl;
    cout << "  7 - запустить нагрузчик" << endl;
    cout << "  8 - выход" << endl;
    cout << "-----------------------------" << endl;

    int cmd, tid, pval;
    HANDLE hload = NULL;

    while (true) {
        cin >> cmd;

        if (cmd == 8) {
            break;
        }
        else if (cmd == 7) {
            if (!load_run) {
                load_run = true;
                load_stop = false;
                
                for (int i = 0; i < 3; i++) {
                    saved_prio[i] = GetThreadPriority(th[i]);
                    SetThreadPriority(th[i], THREAD_PRIORITY_IDLE);
                }
                
                hload = CreateThread(NULL, 0, loader, NULL, 0, NULL);
            }
            else {
                cout << "нагрузчик уже запущен" << endl;
            }
        }
        else {
            cin >> tid >> pval;
            set_pr(tid, pval);
        }
    }

    for (int i = 0; i < 3; i++) {
        run[i] = false;
    }

    load_stop = true;

    WaitForSingleObject(th[0], 1000);
    WaitForSingleObject(th[1], 1000);
    WaitForSingleObject(th[2], 1000);

    if (hload != NULL) {
        WaitForSingleObject(hload, 1000);
        CloseHandle(hload);
    }

    CloseHandle(th[0]);
    CloseHandle(th[1]);
    CloseHandle(th[2]);
    CloseHandle(hlog);

    cout << "программа завершена" << endl;

    return 0;
}
