#include "queue.h"


DWORD WINAPI producer(LPVOID param) {
	queue* q = (queue*)param;
	char data_a[10] = "ABC";
	while (true) {
		enqueue(q, data_a);
		Sleep(300);
	}
}

DWORD WINAPI consumer(LPVOID param) {
	queue* q = (queue*)param;
	while (true) {
		char buffer[10];
		Sleep(10000);
		dequeue(q, buffer);
	}
}



int main() {
	queue* q = create_queue(7);

	HANDLE hProducer;
	HANDLE hConsumer;
	DWORD ProducerID;
	DWORD ConsumerID;

	hProducer = CreateThread(NULL, 0, &producer, (LPVOID)q, 0, &ProducerID);
	hConsumer = CreateThread(NULL, 0, &consumer, (LPVOID)q, 0, &ConsumerID);


	if (hConsumer)
		WaitForSingleObject(hConsumer, INFINITE);
	if (hProducer)
		WaitForSingleObject(hProducer, INFINITE);


	return 0;
}