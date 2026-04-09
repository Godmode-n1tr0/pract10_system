#include <iostream>
#include <Windows.h>
#include <conio.h>
#include <vector>

using namespace std;

HANDLE Work1 = NULL;
HANDLE Work2 = NULL;
HANDLE Work3 = NULL;
HANDLE Work4 = NULL;
HANDLE Counter = NULL;

volatile int result1 = 0;
volatile int result2 = 0;
volatile int result3 = 0;
volatile int result4 = 0;

int savedPrio1 = 0;
int savedPrio2 = 0;
int savedPrio3 = 0;

std::vector<DWORD> Exit;

bool schet = false;

bool start = false;

int Max = 1000;

int TimeOut = 2000;


DWORD WINAPI COUNT(LPVOID smg)
{
	while (true)
	{
		if (result1 != 0)
			cout << result1;
		if (Work4 != NULL)
			cout << " приоритет: " << GetThreadPriority(Work1);
		cout << endl;

		if (result2 != 0)
			cout << result2;
		if (Work4 != NULL)
			cout << " приоритет: " << GetThreadPriority(Work2);
		cout << endl;

		if (result3 != 0)
			cout << result3;
		if (Work4 != NULL)
			cout << " приоритет: " << GetThreadPriority(Work3);
		cout << endl;

		if (result4 != 0)
		{
			cout << result4;
			if (Work4 != NULL)
				cout << " приоритет: " << GetThreadPriority(Work4);
			cout << endl;
		}
		cout << "" << endl;
		Sleep(TimeOut);

	}
}
DWORD WINAPI Fibonachi1(LPVOID smg)
{
	int counter = 0;
	int max = Max;
	int yieldCounter = 0;

	while (true)
	{
		counter++;

		if (counter == max)
		{
			counter = 0;
			result1++;
		}


		//yieldCounter++;
		//if (yieldCounter >= 1000) {
		//	Sleep(0);
		//	yieldCounter = 0;
		//}
	}
}

DWORD WINAPI Fibonachi2(LPVOID smg)
{
	int counter = 0;
	int max = Max;
	int yieldCounter = 0;

	while (true)
	{
		counter++;

		if (counter == max)
		{
			counter = 0;
			result2++;
		}


		//yieldCounter++;
		//if (yieldCounter >= 1000) {
		//	Sleep(0); 
		//	yieldCounter = 0;
		//}
	}
}

DWORD WINAPI Fibonachi3(LPVOID smg)
{
	int counter = 0;
	int max = Max;
	int yieldCounter = 0;

	while (true)
	{
		counter++;

		if (counter == max)
		{
			counter = 0;
			result3++;
		}


		//yieldCounter++;
		//if (yieldCounter >= 1000) {
		//	Sleep(0); 
		//	yieldCounter = 0;
		//}
	}
}

DWORD WINAPI Fibonachi4(LPVOID smg)
{
	int counter = 0;
	int max = Max;
	int yieldCounter = 0;

	while (true)
	{
		counter++;

		if (counter == max)
		{
			counter = 0;
			result4++;
		}


		//yieldCounter++;
		//if (yieldCounter >= 1000) {
		//	Sleep(0);
		//	yieldCounter = 0;
		//}
	}
}
void ConfirmChangePriority(int priority, int id)
{
	switch (id)
	{
	case 0:
		if (Work1 != NULL)
			SetThreadPriority(Work1, priority);
		break;
	case 1:
		if (Work2 != NULL)
			SetThreadPriority(Work2, priority);
		break;
	case 2:
		if (Work3 != NULL)
			SetThreadPriority(Work3, priority);
		break;
	case 3:
		if (Work4 != NULL)
			SetThreadPriority(Work4, priority);
		break;
	}

	result1 = 0; result2 = 0; result3 = 0; result4 = 0;

}

void ChangePriority(int id)
{
	system("cls");
	int choise_priority;
	cout << "Какой приоритет выбираете\n1.Фоновый\n2.низкий\n3.ниже нормы\n4.нормальный\n5.выше нормы\n6.высший\n";
	cin >> choise_priority;

	int priority = THREAD_PRIORITY_NORMAL;

	switch (choise_priority)
	{
	case 1: { ConfirmChangePriority(THREAD_PRIORITY_IDLE, id); break; }
	case 2: { ConfirmChangePriority(THREAD_PRIORITY_LOWEST, id); break; }
	case 3: { ConfirmChangePriority(THREAD_PRIORITY_BELOW_NORMAL, id); break; }
	case 4: { ConfirmChangePriority(THREAD_PRIORITY_NORMAL, id); break; }
	case 5: { ConfirmChangePriority(THREAD_PRIORITY_ABOVE_NORMAL, id); break; }
	case 6: { ConfirmChangePriority(THREAD_PRIORITY_HIGHEST, id); break; }
	default:
		break;
	}
}

int main()
{
	setlocale(0, "rus");
	SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);

	BOOL SSS;
	GetProcessPriorityBoost(GetCurrentProcess(), &SSS);
	Sleep(2000);
	SetProcessPriorityBoost(GetCurrentProcess(), FALSE);
	GetProcessPriorityBoost(GetCurrentProcess(), &SSS);
	Sleep(2000);

	int choise = 0;
	DWORD Id;

	Work1 = CreateThread(NULL, 0, Fibonachi1, (LPVOID)(INT_PTR)0, 0, &Id);
	Exit.push_back(Id);

	Work2 = CreateThread(NULL, 0, Fibonachi2, (LPVOID)(INT_PTR)1, 0, &Id);
	Exit.push_back(Id);

	Work3 = CreateThread(NULL, 0, Fibonachi3, (LPVOID)(INT_PTR)2, 0, &Id);
	Exit.push_back(Id);

	SetThreadPriority(Work1, THREAD_PRIORITY_IDLE);
	SetThreadPriorityBoost(Work1, TRUE);
	SetThreadPriority(Work2, THREAD_PRIORITY_BELOW_NORMAL);
	SetThreadPriorityBoost(Work2, TRUE);
	SetThreadPriority(Work3, THREAD_PRIORITY_NORMAL);
	SetThreadPriorityBoost(Work3, TRUE);

	Exit.push_back(Id);
	schet = true;

	Counter = CreateThread(NULL, 0, COUNT, 0, 0, &Id);

	while (true)
	{
		system("cls");

		cout << "1. Поменять приоритет потока 1 текущий: " << GetThreadPriority(Work1) << endl;
		cout << "2. Поменять приоритет потока 2 текущий: " << GetThreadPriority(Work2) << endl;
		cout << "3. Поменять приоритет потока 3 текущий: " << GetThreadPriority(Work3) << endl;
		cout << "4. Запустить Нагрузчик" << endl;
		cout << "5. Запустить счетчик" << endl;

		cin >> choise;

		switch (choise)
		{
		case 1: { ChangePriority(0); break; }
		case 2: { ChangePriority(1); break; }
		case 3: { ChangePriority(2); break; }
		case 4:
		{
			if (Work4 != NULL)
			{
				cout << "Нагрузчик уже запущен!" << endl;
				Sleep(1000);
				break;
			}

			result1 = 0; result2 = 0; result3 = 0; result4 = 0;


			savedPrio1 = GetThreadPriority(Work1);
			savedPrio2 = GetThreadPriority(Work2);
			savedPrio3 = GetThreadPriority(Work3);

			SetThreadPriority(Work1, THREAD_PRIORITY_IDLE);
			SetThreadPriorityBoost(Work1, TRUE);
			SetThreadPriority(Work2, THREAD_PRIORITY_IDLE);
			SetThreadPriorityBoost(Work2, TRUE);
			SetThreadPriority(Work3, THREAD_PRIORITY_IDLE);
			SetThreadPriorityBoost(Work3, TRUE);

			Work4 = CreateThread(NULL, 0, Fibonachi4, (LPVOID)(INT_PTR)3, 0, &Id);
			Exit.push_back(Id);

			SetThreadPriority(Work4, THREAD_PRIORITY_TIME_CRITICAL);
			SetThreadPriorityBoost(Work4, FALSE);

			TimeOut = 500;


			Sleep(15000);

			if (Work4 != NULL) {
				SetThreadPriority(Work4, THREAD_PRIORITY_NORMAL);
			}

			SetThreadPriority(Work1, savedPrio1);
			SetThreadPriorityBoost(Work1, TRUE);
			SetThreadPriority(Work2, savedPrio2);
			SetThreadPriorityBoost(Work2, TRUE);
			SetThreadPriority(Work3, savedPrio3);
			SetThreadPriorityBoost(Work3, TRUE);

			TimeOut = 2000;

			break;
		}
		case 5: {
			start = true;
			break;
		}
		default:
		{
			break;
		}
		}

	}
}
