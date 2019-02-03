#ifndef _XOPEN_SOURCE 
#define _XOPEN_SOURCE 500
#endif

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <semaphore.h>
#include <sys/shm.h>/*shm*/
#include <errno.h>
#include "msgqueue.h"
#include "msgStruct.h"

#define BUFF 0
#define SIZE 250
#define SHMFLG IPC_CREAT|0666

int GetQueue(key_t _key, pid_t _pId)
{
	int qId;

	qId = msgget(_key, IPC_CREAT|0666);
	if (-1 == qId)
	{
		return MSG_FAILED;
	}

	printf("Proccess id %d connected to queue %d\n",_pId,qId);
	return qId;
}


msgRet SendMessage(int _qId, void* _msg,int _type, size_t _size)
{

	int err;

	err = msgsnd(_qId, _msg, _size, 0);
	if (-1 == err)
	{
		return  MSG_FAILED;
	}

	return MSG_SUCCESS;	
}	

msgRet RecieveMessage(int _qId, void* _msg, int _type, size_t _size)
{
	int err;

	if (NULL == _msg)
	{
		return -1;
	}

	err = msgrcv(_qId, _msg, _size, _type, 0);
	if (-1 == err)
	{
		return  MSG_FAILED;

	}

	return MSG_SUCCESS;

}

msgRet ExitQueue(int _qId)
{
	if (-1 == msgctl(_qId, IPC_RMID, BUFF))
	{
		return MSG_FAILED;
	}

	return MSG_SUCCESS;
}




int* getSharedMemory(key_t _key)
{
	int* shared_M;
	int shmID;

	/* shmget - allocates a shared memory segment */
	if ( -1 == (shmID = shmget(_key, sizeof(int*) , SHMFLG)))
	{
		perror("shmget: shmget failed");
		return NULL;
	}

	/* shmat() attaches the shared memory segment */
	shared_M = shmat(shmID, NULL, 0);
	if ((int*) -1 == shared_M)
	{
		perror("shmat");
        return NULL; 
	}

	return shared_M;
}




