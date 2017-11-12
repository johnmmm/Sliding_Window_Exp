#include "sysinclude.h"
#include <queue>
#include <iostream>

extern void SendFRAMEPacket(unsigned char* pData, unsigned int len);

#define WINDOW_SIZE_STOP_WAIT 1
#define WINDOW_SIZE_BACK_N_FRAME 4

typedef enum {data,ack,nak} frame_kind;
typedef struct frame_head
{
	frame_kind kind;		
	unsigned int seq;
	unsigned int ack;
	unsigned char data[100];
};
typedef struct frame
{
	frame_head head;
	unsigned int size;
};
typedef struct packet
{
	frame* pB;
	unsigned int size;
};
std::deque<packet> buffer_queue;
bool window_full = true;	//To record whether the window is full
int window_size = 0;

/*
* 停等协议测试函数
*/
int stud_slide_window_stop_and_wait(char *pBuffer, int bufferSize, UINT8 messageType)
{
	if (messageType == MSG_TYPE_SEND)
	{
		printf("timeout?\n");
		packet need_to_push;
		need_to_push.pB = new frame();
		need_to_push.pB = (frame*)pBuffer;
		need_to_push.size = bufferSize;
		buffer_queue.push_back(need_to_push);
		if (!window_full)		//if the window is not full
		{
			window_full = true;
			packet buffer_to_send;
			buffer_to_send = buffer_queue.front();
			SendFRAMEPacket((unsigned char*)buffer_to_send.pB, buffer_to_send.size);
		}
	}
	if (messageType == MSG_TYPE_RECEIVE)
	{
		printf("timeout1?\n");
		unsigned int ack_num = ((frame*)pBuffer)->head.ack;
		if (buffer_queue.size() > 0)
		{
			unsigned int seq_num = buffer_queue.front().pB->head.seq;
			if (ack_num == seq_num)		//if receive the right one
			{
				buffer_queue.pop_front();
				window_full = false;	//can open a new window
				if (buffer_queue.size() > 0)	//if there is another buffer, then send it.
				{
					window_full = true;
					packet buffer_to_send;
					buffer_to_send = buffer_queue.front();
					SendFRAMEPacket((unsigned char*)buffer_to_send.pB, buffer_to_send.size);
				}
			}
		}
	}
	if (messageType == MSG_TYPE_TIMEOUT)	//send the buffer again?
	{
		printf("timeout2?\n");
		packet buffer_to_send;
		buffer_to_send = buffer_queue.front();
		SendFRAMEPacket((unsigned char*)buffer_to_send.pB, buffer_to_send.size);
	}
	cout << messageType << endl;
	return 0;
}

/*
* 回退n帧测试函数
*/
int stud_slide_window_back_n_frame(char *pBuffer, int bufferSize, UINT8 messageType)
{
	if (messageType == MSG_TYPE_SEND)
	{
		
	}
	else if (messageType == MSG_TYPE_RECEIVE)
	{
		
	}
	else if (messageType == MSG_TYPE_TIMEOUT)
	{
		
	}
	return 0;
}

/*
* 选择性重传测试函数
*/
int stud_slide_window_choice_frame_resend(char *pBuffer, int bufferSize, UINT8 messageType)
{
	return 0;
}
