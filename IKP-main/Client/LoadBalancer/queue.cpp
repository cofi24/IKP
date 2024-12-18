#include "queue.h"

#define  _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)

HANDLE hSemaporeQueueEmpty;
HANDLE hSemaporeQueueFull;

#define DEBUG

queue* create_queue(int capacity) {
	queue* newQueue = (queue*)malloc(sizeof(queue));
	newQueue->front = 0;
	newQueue->rear = capacity - 1;
	newQueue->capacity = capacity;
	newQueue->currentSize = 0;

	//create array of messages
	newQueue->messageArray = (char**)malloc(sizeof(char*) * capacity);

	for (int i = 0; i < capacity; i++) {
		newQueue->messageArray[i] = NULL;
	}

	InitializeCriticalSection(&newQueue->cs);
	hSemaporeQueueEmpty = CreateSemaphore(0, capacity, capacity, NULL); //empty slots
	hSemaporeQueueFull = CreateSemaphore(0, 0, capacity, NULL); //taken slots

	return newQueue;
}

int is_full(queue* q)
{
	return (q->currentSize == q->capacity);
}

int is_empty(queue* q)
{
	return (q->currentSize == 0);
}

int get_current_size(queue* q) {
	return q->currentSize;
}

int get_capacity(queue* q) {
	return q->capacity;
}

void enqueue(queue* q, char* message) {
	while (is_full(q)) {
		if (WaitForSingleObject(hSemaporeQueueEmpty, INFINITE) == WAIT_OBJECT_0 + 1)
			break;
	}
	{
		EnterCriticalSection(&q->cs);
		//The front and rear will go around like in a circular buffer
		q->rear = (q->rear + 1) % q->capacity;
		q->messageArray[q->rear] = (char*)malloc(strlen(message) + 1);

		//Write at that place the new message
		strcpy(q->messageArray[q->rear], message);
		q->currentSize++;
		#ifdef DEBUG
		print_queue(q);
		#endif
		ReleaseSemaphore(hSemaporeQueueFull, 1, NULL); // increase the number of filled spots
		LeaveCriticalSection(&q->cs);
	}

}

void dequeue(queue* q, char* message) {
	while (q->currentSize == 0) {
		if (WaitForSingleObject(hSemaporeQueueFull, INFINITE) == WAIT_OBJECT_0 + 1)
			break;//The queue is full, wait for elements to be dequeued
	}

	{
		EnterCriticalSection(&q->cs);
		q->messageArray[q->front] = NULL;
		strcpy(message, q->messageArray[q->front]);

		free(q->messageArray[q->front]);
		q->front = (q->front + 1) % q->capacity;
		q->currentSize--;
		if (q->currentSize == 0) {
			q->front = 0;
			q->rear = q->capacity - 1;
		}
		#ifdef DEBUG
		print_queue(q);
		#endif
		ReleaseSemaphore(hSemaporeQueueEmpty, 1, NULL); // increase the number of empty
		LeaveCriticalSection(&q->cs);
	}
}


void delete_queue(queue* q) {
	for (int i = q->front; i <= q->rear; i++) {
		free(q->messageArray[i]);
	}
	free(q->messageArray);
	DeleteCriticalSection(&q->cs);
	free(q);
	CloseHandle(hSemaporeQueueEmpty);
	CloseHandle(hSemaporeQueueFull);
}

void print_queue(queue* q) {
	printf("\n QUEUE: ");

	for (int i = q->front; q->messageArray[i] != NULL;) {
		if (i == q->capacity)
			i = i % q->capacity;
		printf(" %s ", q->messageArray[i]);
		if (i == q->rear)break;
		++i;
	}
	


	printf("\n");
}