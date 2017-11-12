#include "sysinclude.h"
#include <queue>
#include <iostream>

extern void SendFRAMEPacket(unsigned char* pData, unsigned int len);

#define WINDOW_SIZE_STOP_WAIT 1	//actually it can be judged by bool.
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

bool window_full = false;	//To record whether the window is full
/*
* 停等协议测试函数
*/
int stud_slide_window_stop_and_wait(char *pBuffer, int bufferSize, UINT8 messageType)
{
	if (messageType == MSG_TYPE_SEND)
	{
		packet need_to_push;
		need_to_push.pB = new frame();
		*need_to_push.pB = *(frame*)pBuffer;
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
		packet buffer_to_send;
		buffer_to_send = buffer_queue.front();
		SendFRAMEPacket((unsigned char*)buffer_to_send.pB, buffer_to_send.size);
	}
	return 0;
}

int window_used = 0;
/*
* 回退n帧测试函数
*/
int stud_slide_window_back_n_frame(char *pBuffer, int bufferSize, UINT8 messageType)
{
	if (messageType == MSG_TYPE_SEND)
	{
		packet need_to_push;
		need_to_push.pB = new frame();
		*need_to_push.pB = *(frame*)pBuffer;
		need_to_push.size = bufferSize;
		buffer_queue.push_back(need_to_push);
		if (window_used < WINDOW_SIZE_BACK_N_FRAME)
		{
			packet buffer_to_send;
			buffer_to_send = buffer_queue.at(window_used);
			window_used++;
			SendFRAMEPacket((unsigned char*)buffer_to_send.pB, buffer_to_send.size);
		}
	}
	if (messageType == MSG_TYPE_RECEIVE)
	{
		cout << "timeout1" << endl;
		unsigned int ack_num = ((frame*)pBuffer)->head.ack;
		int rece_num = 0;		//to record the id of the buffer that received
		for (int i = 0; i < WINDOW_SIZE_BACK_N_FRAME; i++)
		{
			unsigned int seq_num = buffer_queue.front().pB->head.seq;
			if (ack_num == seq_num)
				rece_num = i;
		}
		for (int i = 0; i < rece_num; i++)	//delete those buffers that has been received
		{
			window_used--;
			buffer_queue.pop_front();
		}
		for (int i = window_used; i < WINDOW_SIZE_BACK_N_FRAME; i++)
		{
			if (buffer_queue.size() > window_used)
			{
				packet buffer_to_send;
				buffer_to_send = buffer_queue.at(window_used);
				window_used++;
				SendFRAMEPacket((unsigned char*)buffer_to_send.pB, buffer_to_send.size);
			}
			else
				break;
		}
	}
	if (messageType == MSG_TYPE_TIMEOUT)
	{
		cout << "timeout2" << endl;
		for (int i = 0; i < WINDOW_SIZE_BACK_N_FRAME; i++)
		{
			packet buffer_to_send;
			buffer_to_send = buffer_queue.at(window_used);
			SendFRAMEPacket((unsigned char*)buffer_to_send.pB, buffer_to_send.size);
		}
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
