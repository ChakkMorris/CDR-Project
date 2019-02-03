#ifndef _XOPEN_SOURCE 
#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <signal.h>
#include <sys/stat.h>
#include <assert.h>
#include <pthread.h>
#include <semaphore.h>


#include "msgqueue.h"
#include "aggregator.h"
#include "SafeHash.h"
#include "msgStruct.h"
#include "commands.h"
#include "dbmanager.h"

#define EQUAL 1
#define NOT_EQUAL 0

#define RECORD 1 /*msg type */
#define FILE_NAME "./file.txt"
#define MSISDN_SIZE 15
#define BRAND_SIZE 10
#define FILE_SIZE 30
#define LINE_LENGTH 1024
#define PATHNAME "../cdr_files/"
#define SIZE 200
#define ACTIONS 1
#define OUTCOME 2
#define CLIENT_MMCMNC 42501 /* partner */
#define SUB_CAPACITY 6000000
#define OP_CAPACITY 5
#define NUM_THREADS 3


/*********** runMsgHandler	*****************/
void* runMsgHandler(void* _agg)
{
	t_records* msg;
	
	key_t key;
	int qId;
	pid_t pId = getpid();	
	key = ftok(FILE_NAME, 'a');
	if (-1 == key)
	{
		perror("Ftok_Fail");
	}

	qId = GetQueue(key, pId);
	((Aggregator*)_agg)->m_readerMsgQueue = qId;
	
	for(;;)
	{
		pthread_mutex_lock(&((Aggregator*)_agg)->m_flagMutex);
	
		while (0 == ((Aggregator*)_agg)->m_runningFlag) /* Puase while flag == 0, resume work else */
		{
			pthread_cond_wait(&((Aggregator*)_agg)->m_runningCond, &((Aggregator*)_agg)->m_flagMutex);
		}	

		msg = (t_records*) calloc (1 , sizeof(t_records));
		if (NULL == msg)
		{
			perror("Calloc_Fail");
		}

		msg->m_type = RECORD;

		if (MSG_FAILED == RecieveMessage(qId, msg, RECORD, sizeof(*msg)))
		{
			perror("MESSAGE_Fail");	
		}
		
		pthread_mutex_unlock(&((Aggregator*)_agg)->m_flagMutex);

		if (DB_ERROR == insertToDB(msg, ((Aggregator*)_agg)->m_DBmanager))
		{
				perror("DB_Insertion_Fail");	 
		}

		if (1 == *((Aggregator*)_agg)->m_sharedMem)
		{
			--((Aggregator*)_agg)->m_aggregatorCounter;
			break;
		}	
	}

	if (0 == ((Aggregator*)_agg)->m_aggregatorCounter)
	{
		*((Aggregator*)_agg)->m_sharedMem = 2;
		if (-1 == sem_post(&((Aggregator*)_agg)->m_shutdownSem))
		{
			perror("m_shutdownSem fail");
       		exit(1);
		}
	}

	return NULL;
}


/*@breif Initialize the Aggregator structure, containing DS's, ipc devices, flags etc 
*********** createAggregator	*****************/
Aggregator* createAggregator(size_t _num, size_t subCapacity, size_t operatorCapacity)
{
	Aggregator* agg;
	DB* hash;
	key_t key;
	int* shutDownFlag;

	agg = (Aggregator*) malloc (sizeof(Aggregator));
	if (NULL == agg)
	{
		return NULL;
	}
		
	if ( 0 != pthread_mutex_init(&agg->m_flagMutex, NULL))
	{

		free(agg);
		perror("mutex fail");
		return NULL;
	}
	
	if (0 != pthread_cond_init(&agg->m_runningCond, NULL))
	{
		pthread_mutex_destroy(&agg->m_flagMutex);
		free(agg);
		return NULL;
	}


	if (-1 ==  sem_init(&agg->m_shutdownSem,  1, 0))
	{
		perror("m_shutdownSem fail");
		pthread_mutex_destroy(&agg->m_flagMutex);
		pthread_cond_destroy(&agg->m_runningCond);
		free(agg);

		return NULL;
    }

	hash = CreateDatabases(	subCapacity, operatorCapacity);
		
	if (NULL == hash)
	{
		pthread_mutex_destroy(&agg->m_flagMutex);
		pthread_cond_destroy(&agg->m_runningCond);
		sem_destroy(&agg->m_shutdownSem);
		free(agg);

		return NULL;
	}

	key = ftok(FILE_NAME, 'b');
	if (-1 == key)
	{
		perror("Key_Fail");
	}

	shutDownFlag = getSharedMemory(key);

	agg->m_sharedMem = shutDownFlag;
	agg->m_DBmanager = hash;
	agg->m_runningFlag = 1;
	agg->m_aggregatorCounter = 0;
	agg->m_numThreads = _num;

	return agg;
}


/*********** executeCommand	*****************
@breif executeCommand executes commands recieved from UI via message queue.*/	
static retAgg executeCommand(Aggregator* _agg, command* _msg, command* _returnMsg) 
{
	switch(_msg->m_command)
	{
		case 1:
				PuaseProcessor(_agg, _returnMsg);
		break;
				
		case 2:
				ResumeProcessor(_agg, _returnMsg);
		break;
	
		case 3:
				SubscriberQuery(_agg->m_DBmanager, _msg, _returnMsg);
		break;
		
		case 4:		
				OperatorQuery(_agg->m_DBmanager, _msg, _returnMsg);
		break;
		
		case 5:
				SubReport(_agg->m_DBmanager,  _returnMsg);
		break;
		
		case 6: 
				OperatorReport(_agg->m_DBmanager,  _returnMsg);                        
		break;
		
		case 7:
				Shutdown(_agg,  _returnMsg);
		break;
	}
	return AGG_SUCCESS;
}


/*********** activateCommandHandler	*****************/
void* RunCommandHandler(void* _agg)
{
	key_t key;
	command msg;
	command returnMsg;
	
	int qId;
	pid_t pId = getpid();
	key = ftok(FILE_NAME, 'c');
	if (-1 == key)
	{
		perror("Key_Fail");
	}
	
	qId = GetQueue(key, pId);
	
	((Aggregator*)_agg)->m_commandMsgQueue = qId;

	for(;;) 
	{
		if (0 == ((Aggregator*)_agg)->m_aggregatorCounter)
		{
			break;
		}		

		
		if (MSG_FAILED == RecieveMessage(qId, &msg, ACTIONS, sizeof(msg)))
		{
			perror("Command_Fail");
			break;
		}
	
		if (AGG_SUCCESS != executeCommand(_agg, &msg, &returnMsg))
		{
			perror("Command_Fail");
		}
		
		
		returnMsg.m_type = OUTCOME;

		if (MSG_FAILED == SendMessage(qId, &returnMsg, OUTCOME, sizeof(command)))
		{
			perror("Message_Fail");
		}

	}
	
	return NULL;
}

/*********** CreateThreads	*****************/
static retAgg CreateThreads(Aggregator* _agg)
{
	pthread_t handlers[_agg->m_numThreads];
	pthread_t commandHandler;
	int i, status;


	for (i=0 ; i<_agg->m_numThreads; i++)
	{
		status = pthread_create(&handlers[i],NULL, runMsgHandler, (void*)_agg);
		if(status != 0)
		{
			perror("thread_error");
		}	
		
		++_agg->m_aggregatorCounter;
	}
	
	status = pthread_create(&commandHandler,NULL, RunCommandHandler, (void*)_agg);
	if(status != 0)
	{
		perror("thread_error");
	}
	
	
	for (i = 0; i<_agg->m_numThreads; i++)
	{
		pthread_join(handlers[i], NULL);
	}
	pthread_join(commandHandler, NULL);
	
	return AGG_SUCCESS;
}

/*********** main	*****************/
int main()
{
	Aggregator* agg;

	if (COMMAND_FAIL == setSignals())
	{
		perror("SIGNAL_CREATION_FAIL");
	}

	agg = createAggregator(NUM_THREADS, SUB_CAPACITY, OP_CAPACITY);
	if (NULL == agg)
	{
		return -1;
	}
	
	if (AGG_SUCCESS != CreateThreads(agg))
	{
		perror("thread_Fail");
	}	
		
}

#endif
