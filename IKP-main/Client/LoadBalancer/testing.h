#pragma once
#include <windows.h>
void test_ht();
void test_hashing();
void test_dynamic_enqueue_dequeue();
void test_list();
DWORD WINAPI producer(LPVOID param);
DWORD WINAPI consumer(LPVOID param);