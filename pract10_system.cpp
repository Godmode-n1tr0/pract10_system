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

// ==========================================// Переработанный поток "Нагрузчик"
// ==========================================
DWORD WINAPI loader(LPVOID p) {
    cout << "[Нагрузчик] Запущен. Приоритет: максимальный (TIME_CRITICAL)" << endl;
    cout << "[Нагрузчик] Начинаю интенсивную нагрузку, выдавливая другие потоки..." << endl;

    // Фаза 1: Максимальный приоритет
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

    volatile double w = 0.0;
    const int phase1_iterations = 300000000; // ~2-4 сек на современном CPU
    
    for (int i = 0; i < phase1_iterations && !load_stop; ++i) {
        w += sqrt((double)i) * sin((double)i) * cos((double)i);
    }

    if (load_stop) {
        cout << "[Нагрузчик] Остановлен пользователем во время фазы максимального приоритета." << endl;
        load_run = false;
        return 0;
    }

    // Переход к нормальному приоритету
    cout << "[Нагрузчик] Фаза выдавливания завершена. Переключаю приоритет на нормальный (NORMAL)..." << endl;
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
    cout << "[Нагрузчик] Работаю с нормальным приоритетом. Потоки делят CPU честно." << endl;

    // Фаза 2: Нормальный приоритет (работает до явной остановки)
    while (!load_stop) {
        for (int i = 0; i < 100000000 && !load_stop; ++i) {
            w += sqrt((double)i) * sin((double)i) * cos((double)i);
        }
        Sleep(10); // Позволяет планировщику Windows корректно обрабатывать прерывание
    }

    cout << "[Нагрузчик] Работа завершена по команде пользователя." << endl;
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
            if (pr == THREAD_PRIORITY_LOWEST) pr_str = "низкий";
            else if (pr == THREAD_PRIORITY_BELOW_NORMAL) pr_str = "ниже нормального";
            else if (pr == THREAD_PRIORITY_NORMAL) pr_str = "нормальный";
            else if (pr == THREAD_PRIORITY_ABOVE_NORMAL) pr_str = "выше нормального";
            else if (pr == THREAD_PRIORITY_HIGHEST) pr_str = "высокий";
            else if (pr == THREAD_PRIORITY_TIME_CRITICAL) pr_str = "максимальный";
            else if (pr == THREAD_PRIORITY_IDLE) pr_str = "минимальный";

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
    case 1: pr = THREAD_PRIORITY_LOWEST; break;
    case 2: pr = THREAD_PRIORITY_BELOW_NORMAL; break;
    case 3: pr = THREAD_PRIORITY_NORMAL; break;
    case 4: pr = THREAD_PRIORITY_ABOVE_NORMAL; break;
    case 5: pr = THREAD_PRIORITY_HIGHEST; break;
    case 6: pr = THREAD_PRIORITY_TIME_CRITICAL; break;
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

    SetThreadPriority(th[0], THREAD_PRIORITY_LOWEST);
    SetThreadPriority(th[1], THREAD_PRIORITY_NORMAL);
    SetThreadPriority(th[2], THREAD_PRIORITY_HIGHEST);

    HANDLE hlog = CreateThread(NULL, 0, logger, NULL, 0, NULL);

    cout << "программа запущена" << endl;
    cout << "-----------------------------" << endl;    cout << "1 - низкий" << endl;
    cout << "2 - ниже нормального" << endl;
    cout << "3 - нормальный" << endl;
    cout << "4 - выше нормального" << endl;
    cout << "5 - высокий" << endl;
    cout << "6 - максимальный" << endl;
    cout << "-----------------------------" << endl;
    cout << "команды:" << endl;
    cout << "  номер_потока приоритет" << endl;
    cout << "  пример: 0 3" << endl;
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

    if (hload != NULL) {        WaitForSingleObject(hload, 2000); // Увеличили таймаут для корректного завершения нагрузчика
        CloseHandle(hload);
    }

    CloseHandle(th[0]);
    CloseHandle(th[1]);
    CloseHandle(th[2]);
    CloseHandle(hlog);

    cout << "программа завершена" << endl;

    return 0;
}
