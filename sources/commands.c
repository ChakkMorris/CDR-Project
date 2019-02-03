#include <stdio.h>
#include <signal.h>
#include <sys/msg.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <unistd.h>

#include "dbmanager.h"
#include "SafeHash.h"
#include "commands.h"
#include "msgStruct.h"
#include "msgqueue.h"

#define SUBSCRIBERS_SIGNAL 5
#define OPERATORS_SIGNAL   6
#define ACTIONS 1

#define OPQUERY 500
#define SUBQUERY 1000
#define RESUCCESS 60
#define RETFAIL 30
#define FILE_NAME "./file.txt"

static void sigusr1_handler();
static void sigusr2_handler();


/******************		OperatorQuary	 *******************/
commandRet OperatorQuery(DB* _hash, command* _msg, command* _returnMsg)
{
	OperatorRecord* pValue;
	int findResult;
	
	findResult = SafeMap_Find(_hash->m_operatorHash, (void*)&(_msg->m_mmcmnc), (void**)&pValue);
	if (MAP_KEY_NOT_FOUND == findResult)
	{
		snprintf(_returnMsg->m_returnMessage, RETFAIL, "Operator MMC/MNC not in data base\n");
		return COMMAND_SUCCESS;
	}
	
	snprintf(_returnMsg->m_returnMessage, SUBQUERY, "Operator number %d:\nOutgoing call:%lld\nIncoming calls:%lld\nOutgoing SMSs:%lld\nIncoming SMSs:%lld",_msg->m_mmcmnc,pValue->m_outgoing_calls,pValue->m_incoming_calls,pValue->m_incoming_sms,pValue->m_outgoing_sms);

	return COMMAND_SUCCESS;
}



/******************	SubscriberQuary		*******************/
commandRet SubscriberQuery(DB* _hash, command* _msg, command* _returnMsg)
{
	SubRecord* pValue;
	int findResult;
	
	findResult = SafeMap_Find(_hash->m_subHash, (void*)&(_msg->m_msisdn), (void**)&pValue);
	if (MAP_KEY_NOT_FOUND == findResult)
	{
		snprintf(_returnMsg->m_returnMessage, RETFAIL, "Subscriber MSISDN not in data base\n");
		return COMMAND_SUCCESS;
	}

	snprintf(_returnMsg->m_returnMessage, OPQUERY, "Subscriber number %lld:\nOutgoing calls within network:%lld\nIncoming calls within network:%lld\nOutgoing calls outside of network:%lld\nIncoming calls outside of network:%lld\nOutgoing SMSs within network:%lld\nIncoming SMSs withing network:%lld\nOutgoing SMSs outside of network:%lld\nIncoming SMSs outside of network:%lld\nData downlaoded:%f\nData uploaded:%f\n",_msg->m_msisdn,pValue->m_outgoing_within_callDuration,pValue->m_incoming_within_callDuration, pValue->m_outgoing_outside_callDuration, pValue->m_incoming_outside_callDuration, pValue->m_outgoing_within_sms, pValue->m_incoming_within_sms, pValue->m_outgoing_outside_sms, pValue->m_incoming_outside_sms, pValue->m_downloaded, pValue->m_uploaded);
	
	return COMMAND_SUCCESS;
}	


/******************	*******************/
static int PrintOperator(void* _key, void* _value, void* _file)
{
	
	char buffer[OPQUERY];
	
		snprintf(buffer, OPQUERY, "MMC/MNC %d, total incoming call durations %lld, total outgoing call durations %lld, total incoming sms messages %lld, total outgoing sms messages %lld", ((OperatorRecord*)_value)->m_mmcmnc, ((OperatorRecord*)_value)->m_incoming_calls, ((OperatorRecord*)_value)->m_outgoing_calls, ((OperatorRecord*)_value)->m_incoming_sms, ((OperatorRecord*)_value)->m_outgoing_sms);
		
	fprintf((FILE*)_file, "%s\n", buffer);  

	return 1;
}

/****************** 	ReportersReport		*******************/
commandRet OperatorReport(DB* _hash, command* _returnMsg)
{
	size_t count=0;
		
	FILE* file;

	file = fopen("../reports/operatorReport.txt", "w+");
	if (file == NULL)
	{
		return COMMAND_FAIL;
	} 
	
	count = SafeMap_ForEach(_hash->m_operatorHash, (KeyValueActionFunction) PrintOperator, file);
	if (0 == count)
	{
		snprintf(_returnMsg->m_returnMessage, RESUCCESS, "No items in data base. File created is empty");
		return COMMAND_SUCCESS;	
	}

	fclose(file);
	snprintf(_returnMsg->m_returnMessage, RESUCCESS, "Operators settlement file created in reports folder");

	return COMMAND_SUCCESS;
}



/******************  CreateSubReport	*******************/
static int PrintSubscriber(void* _key, void* _value, void* _file) 
{

	char buffer[OPQUERY]; 

	snprintf(buffer, OPQUERY, "msisdn %lld, total incoming calls within network %lld, total incoming calls outside network %lld, total outgoing calls within network %lld, total outgoing calls outside network %lld, total incoming sms messages within network %lld, total incoming sms messages outside network %lld, total outgoing sms messages incoming network %lld, total outgoing sms messages outside network %lld, total downloaded data %f, total uploaded data %f", ((SubRecord*)_value)->m_msisdn, ((SubRecord*)_value)->m_incoming_within_callDuration, ((SubRecord*)_value)->m_incoming_outside_callDuration, ((SubRecord*)_value)->m_outgoing_within_callDuration, ((SubRecord*)_value)->m_outgoing_outside_callDuration, ((SubRecord*)_value)->m_incoming_within_sms, ((SubRecord*)_value)->m_incoming_outside_sms, ((SubRecord*)_value)->m_outgoing_within_sms, ((SubRecord*)_value)->m_outgoing_outside_sms, ((SubRecord*)_value)->m_downloaded, ((SubRecord*)_value)->m_uploaded);	
	
	fprintf((FILE*)_file, "%s\n", buffer);  
	
	return 1;
}

/****************** SubReport *******************/
commandRet SubReport(DB* _hash , command* _returnMsg)
{
	size_t count=0;
		
	FILE* file;

	file = fopen("../reports/SubscribersReport.txt", "w+");
	if (file == NULL)
	{
		return COMMAND_FAIL;
	} 
	
	count = SafeMap_ForEach(_hash->m_subHash, (KeyValueActionFunction) PrintSubscriber, file);
	
	if (0 == count)
	{
		snprintf(_returnMsg->m_returnMessage, RESUCCESS, "No items in data base. File created is empty");
		return COMMAND_SUCCESS;	
	}

	fclose(file);
	snprintf(_returnMsg->m_returnMessage, RESUCCESS, "Subscribers billing report created in reports folder");

	return COMMAND_SUCCESS;
}

/******************  ResumeProcessor *******************/
commandRet PuaseProcessor(Aggregator* _agg, command* _returnMsg)
{
	_agg->m_runningFlag = 0;
	
	snprintf(_returnMsg->m_returnMessage, RESUCCESS, "CDR processor is currently paused\n");	

	return COMMAND_SUCCESS;
}

/******************   ResumeProcessor *******************/
commandRet ResumeProcessor(Aggregator* _agg, command* _returnMsg)
{
	_agg->m_runningFlag = 1;
	pthread_cond_broadcast(&_agg->m_runningCond); 
	
	snprintf(_returnMsg->m_returnMessage, RESUCCESS, "CDR processor is currently running\n");
	return COMMAND_SUCCESS;
}

/******************   Shutdown   *******************/
commandRet Shutdown(Aggregator* _agg, command* _returnMsg)
{

	*_agg->m_sharedMem = 1;
printf("%d\n",*(_agg)->m_sharedMem);

	if (-1 == sem_wait(&_agg->m_shutdownSem))
	{
		perror("m_shutdownSem fail");
		return COMMAND_FAIL;
	}

	msgctl(_agg->m_readerMsgQueue , IPC_RMID, 0);


	snprintf(_returnMsg->m_returnMessage, RESUCCESS, "CDR procesor has been shut down\n");
	
	return COMMAND_SUCCESS;
}


/******************   setSignals *******************/
commandRet setSignals()
{

	struct sigaction sa1, sa2; 

	sa1.sa_flags = SA_SIGINFO;
	sa1.sa_sigaction = sigusr1_handler;
	sigemptyset(&sa1.sa_mask);

	if (-1 == sigaction(SIGUSR1, &sa1, NULL))
	{
		perror("SIGUSR1 handler failed");
		return COMMAND_FAIL;
	}    

	sa2.sa_flags = SA_SIGINFO;
	sa2.sa_sigaction = sigusr2_handler; 
	sigemptyset(&sa1.sa_mask);

	if (-1 == sigaction(SIGUSR2, &sa2, NULL))
	{
		perror("SIGUSR2 handler failed");
		return COMMAND_FAIL;
	}   

	return COMMAND_SUCCESS;
}


/***********    Sig Handlers     *********************/
static void sigusr1_handler(siginfo_t *siginfo)
{
	int qId, err;
	command msg;
	key_t key;
	pid_t pId = getpid();	

	key = ftok(FILE_NAME, 'c');
	if (-1 == key)
	{
		perror("Key_Fail");
	}

	qId = GetQueue(key, pId);

	msg.m_type	= ACTIONS;
	msg.m_command 	= SUBSCRIBERS_SIGNAL;
	
	err = msgsnd(qId, &msg, sizeof(command), 0);
	if (err == -1)
	{
		perror("Message_fail");
	}
}

static void sigusr2_handler(siginfo_t *siginfo)
{
	int qId, err;
	command msg;
	key_t key;
	pid_t pId = getpid();	

	key = ftok(FILE_NAME, 'c');
	if (-1 == key)
	{
		perror("Key_Fail");
	}

	qId = GetQueue(key, pId);

	msg.m_type	= ACTIONS;
	msg.m_command 	= OPERATORS_SIGNAL;
	
	err = msgsnd(qId, &msg, sizeof(command), 0);
	if (err == -1)
	{
		perror("Message_fail");
	}
}






