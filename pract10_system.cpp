#include <windows.h>
#include <iostream>
using namespace std;

volatile long long cnt[3] = { 0, 0, 0 };
volatile bool run[3] = { true, true, true };
volatile bool load_run = false;
volatile bool load_stop = false;

HANDLE th[3] = { null, null, null };
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

DWORD WINAPI loader(LPVOID p) {
    cout << "нагрузчик запущен с приоритетом максимальный" << endl;
    
    setthreadpriority(getcurrentthread(), thread_priority_time_critical);
    
    volatile long long w = 0;
    for (int i = 0; i < 1000000000 && !load_stop; i++) {
        w += i;
    }
    
    setthreadpriority(getcurrentthread(), thread_priority_normal);
    
    cout << "нагрузчик завершил работу, приоритет нормальный" << endl;
    
    load_run = false;
    return 0;
}

DWORD WINAPI logger(LPVOID p) {
    char* names[] = { "инкремент", "фибоначчи", "факториал" };
    
    while (true) {
        sleep(1000);
        
        for (int i = 0; i < 3; i++) {
            int pr = getthreadpriority(th[i]);
            char* pr_str = "нормальный";
            
            if (pr == thread_priority_lowest) pr_str = "низкий";
            else if (pr == thread_priority_below_normal) pr_str = "ниже нормального";
            else if (pr == thread_priority_normal) pr_str = "нормальный";
            else if (pr == thread_priority_above_normal) pr_str = "выше нормального";
            else if (pr == thread_priority_highest) pr_str = "высокий";
            else if (pr == thread_priority_time_critical) pr_str = "максимальный";
            else if (pr == thread_priority_idle) pr_str = "минимальный";
            
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
    
    int pr = thread_priority_normal;
    switch (val) {
        case 1: pr = thread_priority_lowest; break;
        case 2: pr = thread_priority_below_normal; break;
        case 3: pr = thread_priority_normal; break;
        case 4: pr = thread_priority_above_normal; break;
        case 5: pr = thread_priority_highest; break;
        case 6: pr = thread_priority_time_critical; break;
        default: return;
    }
    
    setthreadpriority(th[tid], pr);
    cout << "приоритет потока " << tid << " изменен" << endl;
}

int main() {
    setlocale(0, "rus");
    
    th[0] = createthread(null, 0, inc, (void*)0, 0, &id[0]);
    th[1] = createthread(null, 0, fib, (void*)1, 0, &id[1]);
    th[2] = createthread(null, 0, fact, (void*)2, 0, &id[2]);
    
    setthreadpriority(th[0], thread_priority_lowest);
    setthreadpriority(th[1], thread_priority_normal);
    setthreadpriority(th[2], thread_priority_highest);
    
    HANDLE hlog = createthread(null, 0, logger, null, 0, null);
    
    cout << "программа запущена" << endl;
    cout << "-----------------------------" << endl;
    cout << "1 - низкий" << endl;
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
    HANDLE hload = null;
    
    while (true) {
        cin >> cmd;
        
        if (cmd == 8) {
            break;
        }
        else if (cmd == 7) {
            if (!load_run) {
                load_run = true;
                load_stop = false;
                hload = createthread(null, 0, loader, null, 0, null);
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
    
    waitforsingleobject(th[0], 1000);
    waitforsingleobject(th[1], 1000);
    waitforsingleobject(th[2], 1000);
    
    if (hload != null) {
        waitforsingleobject(hload, 1000);
        closehandle(hload);
    }
    
    closehandle(th[0]);
    closehandle(th[1]);
    closehandle(th[2]);
    closehandle(hlog);
    
    cout << "программа завершена" << endl;
    
    return 0;
}
