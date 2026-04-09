// zad10-sis.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <Windows.h>
#include <conio.h>

volatile long iterations[3] = { 0 };
volatile bool stressRunning = false;

const char* PriorityToString(int prio) {
    switch (prio) {
    case -15: return "IDLE";
    case -2:  return "LOWEST";
    case -1:  return "BELOW_NORMAL";
    case 0:   return "NORMAL";
    case 1:   return "ABOVE_NORMAL";
    case 2:   return "HIGHEST";
    case 15:  return "TIME_CRITICAL";
    default:  return "CUSTOM";
    }
}

DWORD WINAPI Thread0(LPVOID) {
    long a = 0;
    while (true) {
        a++;
        InterlockedIncrement(&iterations[0]);
    }
    return 0;
}

DWORD WINAPI Thread1(LPVOID) {
    long a = 0, b = 1;
    while (true) {
        long c = a + b;
        a = b;
        b = c;
        InterlockedIncrement(&iterations[1]);
    }
    return 0;
}

DWORD WINAPI Thread2(LPVOID) {
    while (true) {
        long f = 1;
        for (int i = 1; i <= 10; ++i) {
            f *= i;
        }
        InterlockedIncrement(&iterations[2]);
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

int main() {
    setlocale(0, "rus");
    std::cout << "Потоки будут запущены со следующими приоритетами:\n";
    std::cout << "Поток 1 - Инкремент: BELOW_NORMAL (-1)\n";
    std::cout << "Поток 2 - Фибоначчи: NORMAL (0)\n";
    std::cout << "Поток 3 - Факториал: ABOVE_NORMAL (1)\n";
    std::cout << "\nНажмите любую клавишу для запуска потоков...\n";
    _getch();

    const char* names[3] = { "Инкремент", "Фибоначчи", "Факториал" };

    HANDLE hThreads[3];
    hThreads[0] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Thread0, NULL, 0, NULL);
    hThreads[1] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Thread1, NULL, 0, NULL);
    hThreads[2] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Thread2, NULL, 0, NULL);

    SetThreadPriority(hThreads[0], -1);
    SetThreadPriority(hThreads[1], 0);
    SetThreadPriority(hThreads[2], 1);

    HANDLE stressThreadHandle = nullptr;
    const int stressPrioritySequence[] = { 15, 2, 1, 0, -1, -2, -15 };
    int currentStressPriorityIndex = 0;
    int stressTimerSec = 0;

    bool quit = false;
    while (!quit) {
        Sleep(1000);
        system("cls");

        std::cout << "Статистика:\n";
        for (int i = 0; i < 3; ++i) {
            int iters = iterations[i];
            iterations[i] = 0;
            int p = GetThreadPriority(hThreads[i]);
            std::cout << "Поток " << (i + 1) << " - " << names[i] << ": " << iters << " итераций | Приоритет: " << PriorityToString(p) << " (" << p << ")\n";
        }

        if (stressRunning && stressThreadHandle != nullptr) {
            int p = GetThreadPriority(stressThreadHandle);
            std::cout << "Поток 4 - Нагрузчик: активен | Приоритет: "
                << PriorityToString(p) << " (" << p << ")\n";
        }

        if (stressRunning && stressThreadHandle != nullptr) {
            stressTimerSec++;
            if (stressTimerSec >= 10) {
                stressTimerSec = 0;
                if (currentStressPriorityIndex < 6) {
                    currentStressPriorityIndex++;
                    SetThreadPriority(stressThreadHandle, stressPrioritySequence[currentStressPriorityIndex]);
                }
            }
        }
        else {
            stressTimerSec = 0;
        }

        std::cout << "\nУправление:\n";
        std::cout << "1 — изменить приоритет Инкремента\n";
        std::cout << "2 — изменить приоритет Фибоначчи\n";
        std::cout << "3 — изменить приоритет Факториала\n";
        std::cout << "4 — запустить/остановить Нагрузчик\n";
        std::cout << "5 — выйти\n";

        if (_kbhit()) {
            char ch = _getch();
            if (ch == '1' || ch == '2' || ch == '3') {
                int idx = ch - '1';
                std::cout << "\nНовый приоритет для потока " << (idx + 1) << ":\n";
                std::cout << "1: IDLE (-15)\n2: LOWEST (-2)\n3: BELOW_NORMAL (-1)\n";
                std::cout << "4: NORMAL (0)\n5: ABOVE_NORMAL (1)\n6: HIGHEST (2)\n7: TIME_CRITICAL (15)\n";
                std::cout << "Выбор: ";
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

                SetThreadPriority(hThreads[idx], prio);
                int confirmed = GetThreadPriority(hThreads[idx]);
                std::cout << "\nУстановлен: " << PriorityToString(confirmed) << " (" << confirmed << ")\n";
                Sleep(1200);
            }
            else if (ch == '4') {
                if (stressRunning) {
                    stressRunning = false;
                    WaitForSingleObject(stressThreadHandle, INFINITE);
                    CloseHandle(stressThreadHandle);
                    stressThreadHandle = nullptr;
                }
                else {
                    stressRunning = true;
                    stressThreadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StressThreadProc, NULL, 0, NULL);
                    currentStressPriorityIndex = 0;
                    SetThreadPriority(stressThreadHandle, 15);
                }
            }
            else if (ch == '5') {
                quit = true;
            }
        }
    }

    if (stressThreadHandle) {
        stressRunning = false;
        WaitForSingleObject(stressThreadHandle, INFINITE);
        CloseHandle(stressThreadHandle);
    }

    for (int i = 0; i < 3; ++i) {
        TerminateThread(hThreads[i], 0);
        CloseHandle(hThreads[i]);
    }
    std::cout << "\nПрограмма завершена.\n";
    return 0;
}
