#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
DWORD WINAPI client_listener(LPVOID param);
DWORD WINAPI client_read_write(LPVOID param);
DWORD WINAPI worker_read(LPVOID param);
DWORD WINAPI worker_write(LPVOID param);
DWORD WINAPI worker_listener(LPVOID param);