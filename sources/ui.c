#ifndef _XOPEN_SOURCE 
#define _XOPEN_SOURCE 500
#endif

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "msgStruct.h"       
#include "msgqueue.h"
#include "ui.h"

#define FILE_NAME "./file.txt"
#define ACTIONS 1
#define OUTCOME 2

#define IRELEVENT 3
#define OPERATOR 1
#define SUBSCRIBER 2



/************* SendCommad **********/
static int SendCommad(int _qId, pid_t _pId, int _action, int _identidy)
{
	int operatorId, err;
	long long subscriberId;
	command msg;
	
	if (OPERATOR == _identidy)
	{
		printf("Enter operator MMC/MNC\n");
		scanf("%d", &operatorId);
		msg.m_mmcmnc = operatorId;
	}
	else if (SUBSCRIBER == _identidy)
	{
		printf("Enter subscriber MSISDN\n");
		scanf("%lld", &subscriberId);
		msg.m_msisdn = subscriberId;
	}
	
	msg.m_type 	  = ACTIONS;
	msg.m_command = _action;

	err = msgsnd(_qId, &msg, sizeof(command), 0);
	if (err == -1)
	{
		perror("msg ssss");
	}

	return 1;
}


/************* main **********/
int main()
{
	int qId, action=1;
	pid_t pId = getpid();
	key_t key;

	command retMsg;
	key = ftok(FILE_NAME, 'c');
	if (-1 == key)
	{
		printf("key fail\n");
	}
	
	qId = GetQueue(key, pId); /* create command message queue */	

	do
	{
		printf("\n\n CDR processor UI.\n"
			 "\n Pause System  			->		1"
			 "\n Resume System 			->		2"
			 "\n MSISN Quary   			->		3"
			 "\n Operator Quary			->		4"
			 "\n All Subscribers Report 	->		5"
			 "\n All Operators Report   	->		6"
			 "\n Shutdown 	   		->		7\n");
			
		printf("Choose action, then press Enter :\n"); 	
		scanf("%d", &action);
		
		if (action < 1 || action > 8)
		{
			printf("\nAction requested not eligble. Try again\n");
			continue;
		}

		 switch(action)
		 {
		 	case 1:
		 		SendCommad(qId, pId, action, IRELEVENT);
		 		break;
		 	
		 	case 2:
		 		SendCommad(qId, pId, action, IRELEVENT);
		 		break;
		 	
		 	case 3:
		 		SendCommad(qId, pId, action, SUBSCRIBER);
		 		break;
		 	
		 	case 4:
		 		SendCommad(qId, pId, action, OPERATOR);
		 		break;
		 	
		 	case 5:
		 		SendCommad(qId, pId, action, IRELEVENT);
		 		break;
		 
		 	case 6:                            
		 		SendCommad(qId, pId, action, IRELEVENT);
	 			break;
		 
		 	case 7:

		 		SendCommad(qId, pId, action, IRELEVENT);
	 			break;
	 	}
	 	
		if (MSG_FAILED == RecieveMessage(qId, &retMsg, OUTCOME, sizeof(command)))
		{
			break;
		}

		printf("%s\n",retMsg.m_returnMessage);
		if (7 == action) /* shutdown - destroy message queue */
		{
			msgctl(qId , IPC_RMID, 0);
			printf("Closing program. Goodbay!");
			exit(1);	
		}
	}
	
	while(action);
		
	return 0;			 
}

