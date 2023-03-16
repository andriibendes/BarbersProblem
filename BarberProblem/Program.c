#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define X 5
#define Y 30
#define C 100


HANDLE waiting_room_mutex;
int waiting_room[Y];
int waiting_count = 0;
int all_clients = C;

DWORD WINAPI barber_work(LPVOID arg)
{
    int barber_id = (int)arg;

    while (TRUE) {

        WaitForSingleObject(waiting_room_mutex, INFINITE);

        if (waiting_count == 0) {

            if (all_clients == 0)
            {
                ReleaseMutex(waiting_room_mutex);
                break;
            }
            printf("Barber %d is waiting for a client\n", barber_id);
            ReleaseMutex(waiting_room_mutex); 
            continue;
        }

        int client_id = waiting_room[--waiting_count];

        ReleaseMutex(waiting_room_mutex);

        printf("Barber %d finished with a client %d\n", barber_id, client_id);
    }
}

DWORD WINAPI client_work(LPVOID arg) 
{
    int client_id = (int)arg;
    WaitForSingleObject(waiting_room_mutex, INFINITE);

    if (waiting_count == Y) 
    {
        printf("Client %d is leaving the barber shop because there are no free chairs\n", client_id);
    }
    else
    {
        waiting_room[waiting_count++] = client_id;
        printf("Client %d is waiting for a barber\n", client_id);
    }
    ReleaseMutex(waiting_room_mutex);
    all_clients--;
}

int main()
{
    HANDLE threads[X + C];

    int barber_ids[X];
    int client_ids[C];

    waiting_room_mutex = CreateMutex(NULL, FALSE, NULL);

    for (int i = 0; i < Y; i++)
    {
        waiting_room[i] = -1;
    }

    for (int i = 0; i < X; i++)
    {
        barber_ids[i] = i;
        threads[i] = CreateThread(NULL, 0, barber_work, barber_ids[i], 0, NULL);
    }

    for (int i = 0; i < C; i++)
    {
        client_ids[i] = i;
        threads[X + i] = CreateThread(NULL, 0, client_work, client_ids[i], 0, NULL);
    }

    WaitForMultipleObjects(X + C, threads, TRUE, INFINITE);

    return 0;
}