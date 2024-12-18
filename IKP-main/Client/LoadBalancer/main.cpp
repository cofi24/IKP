
#include <stdlib.h>
#include <stdio.h>
#include "conio.h"
#include "hash_map.h"
#include "queue.h"
#include "ClientCommunication.h"
#include "list.h"
#include "testing.h"
#include "WorkerCommunication.h"

#define WORKER_IP_ADDRESS "127.0.0.1"
#define WORKER_PORT 6069


#pragma warning(disable:4996)




DWORD WINAPI checkPercentage(LPVOID param) {
    while (true) {
        Sleep(3000);
        int fullfillness = ((float)get_current_size(q) / (float)get_capacity(q)) * 100;
        printf("Queue is at %d%%\n", fullfillness);
        if (fullfillness < 30) {
            //shut down worker threads
        }
        else if (fullfillness > 70) {
           
             // open new worker processes
            //CreateProcess();
        }
    }
}

DWORD WINAPI dispatcher(LPVOID param) {
    char message[256];

    while (true) {
        Sleep(3000);

        if (is_empty(q)) {
            printf("D: Queue is empty.\n");
        }
        else {
            dequeue(q, message);
            node* first = free_workers_list->head;

            strcpy(first->msgBuffer, message);
            ReleaseSemaphore(first->msgSemaphore, 1, NULL);
        }
    }


    return 0;
}
   
   
void create_new_worker_process() {
    STARTUPINFO startup_info;
    PROCESS_INFORMATION process_info;
    memset(&startup_info, 0, sizeof(STARTUPINFO));
    startup_info.cb = sizeof(STARTUPINFO);
    memset(&process_info, 0, sizeof(PROCESS_INFORMATION));
    TCHAR buff[100];
    GetCurrentDirectory(100, buff);
    wcscat(buff, L"\\..\\Debug\\Worker.exe");
    if (!CreateProcess(
        buff,          // LPCTSTR lpApplicationName
        NULL,                // LPTSTR lpCommandLine
        NULL,                // LPSECURITY_ATTRIBUTES lpProcessAttributes
        NULL,                // LPSECURITY_ATTRIBUTES lpThreadAttributes
        FALSE,               // BOOL bInheritHandles
        CREATE_NO_WINDOW,    // DWORD dwCreationFlags
        NULL,                // LPVOID lpEnvironment
        NULL,                // LPCTSTR lpCurrentDirectory
        &startup_info,       // LPSTARTUPINFO lpStartupInfo
        &process_info        // LPPROCESS_INFORMATION lpProcessInformation
    )) {
        printf("CreateProcess failed (%d).\n", GetLastError());
    }
}
  


int main() {

    // Create a listener client thread which handles incoming connections
    HANDLE hListenerClient;
    HANDLE hListenerWorker;
    HANDLE hPercentage;
    HANDLE hDispatcher;
    DWORD listenerClientID;
    DWORD percentageID;
    DWORD listenerWorkerID;
    DWORD dispatcherID;
    init_hash_table();
    q = create_queue(8);
    init_list(&free_workers_list);
    init_list(&busy_workers_list);
    hPercentage = CreateThread(NULL, 0, &checkPercentage, (LPVOID)0, 0, &percentageID);
    hListenerClient = CreateThread(NULL, 0, &client_listener, (LPVOID)q, 0, &listenerClientID);
    hListenerWorker = CreateThread(NULL, 0, &worker_listener, (LPVOID)0, 0, &listenerWorkerID);
    hDispatcher = CreateThread(NULL, 0, &dispatcher, (LPVOID)0, 0, &dispatcherID);
    //wait for listener to finish
    if (hListenerClient)
        WaitForSingleObject(hListenerClient, INFINITE);
    if (hListenerWorker)
        WaitForSingleObject(hListenerWorker, INFINITE);
    if (hPercentage)
        WaitForSingleObject(hPercentage, INFINITE);
    if (hDispatcher)
        WaitForSingleObject(hDispatcher, INFINITE);
    delete_hashtable();

    delete_list(free_workers_list);
    delete_list(busy_workers_list);
    delete_queue(q);
    return 0;
}
