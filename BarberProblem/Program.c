#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define X 3 // кількість перукарів
#define Y 5 // кількість стільців в кімнаті очікування

HANDLE mutex; // м'ютекс
HANDLE cond; // змінна-умова

int clients_waiting = 0; // кількість клієнтів, які очікують
int chairs_left = Y; // кількість вільних стільців в кімнаті очікування

DWORD WINAPI barber_work(LPVOID arg) {
    int barber_id = *(int*)arg;
    while (1) {
        WaitForSingleObject(mutex, INFINITE);
        if (clients_waiting == 0) {
            printf("Barber %d is waiting for a customer\n", barber_id);
            ReleaseMutex(mutex);
            WaitForSingleObject(cond, INFINITE);
        }
        clients_waiting--;
        chairs_left++;
        printf("Barber %d is working on a customer\n", barber_id);
        Sleep(2000); // час, необхідний на обслуговування клієнта
        printf("Barber %d finished with a customer\n", barber_id);
        ReleaseMutex(mutex);
    }
}

DWORD WINAPI client_work(LPVOID arg) {
    while (1) {
        WaitForSingleObject(mutex, INFINITE);
        if (chairs_left == 0) {
            printf("Client is leaving the barber shop because there are no free chairs\n");
            ReleaseMutex(mutex);
            Sleep(rand() % 3000 + 1000); // час, необхідний для переходу до іншої перукарні
            continue;
        }
        chairs_left--;
        clients_waiting++;
        printf("Client is waiting for a barber\n");
        ReleaseMutex(mutex);
        SetEvent(cond);
        Sleep(1000); // час, необхідний для підходу до перукаря
        printf("Client is getting a haircut\n");
        Sleep(2000); // час, необхідний на стрижку волосся
        printf("Client finished getting a haircut\n");
    }
}

int main() {
    HANDLE barbers[X];
    HANDLE clients;
    int barber_ids[X];
    mutex = CreateMutex(NULL, FALSE, NULL);
    cond = CreateEvent(NULL, FALSE, FALSE, NULL);
    for (int i = 0; i < X; i++) {
        barber_ids[i] = i;
        barbers[i] = CreateThread(NULL, 0, barber_work, &barber_ids[i], 0, NULL);
    }
    clients = CreateThread(NULL, 0, client_work, NULL, 0, NULL);
    WaitForSingleObject(clients, INFINITE);
    return 0;
}