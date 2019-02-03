#ifndef _XOPEN_SOURCE 
#define _XOPEN_SOURCE 500
#endif

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<errno.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <signal.h>
#include <sys/stat.h>
#include <assert.h>
#include <unistd.h>/*pid*/
#include "collector.h"
#include "msgqueue.h"
#include "msgStruct.h"

#define RECORD 1 /*msg type */
#define MSISDN_SIZE 15
#define BRAND_SIZE 10
#define FILE_SIZE 30
#define LINE_LENGTH 1024
#define PATHNAME "../cdr_files/"
#define ARCHIVE	"../archive/"
#define FILE_NAME "./file.txt"
#define EMPTY '0'


/************   operatorHandler   ************/
static OperatorRecord operatorHandler(int _i_mmcmnc,int _p_mmcmnc, char* _type, int _duration)
{
	OperatorRecord record;
	memset(&record, 0, sizeof(record));
	record.m_mmcmnc = _p_mmcmnc;



	if (_i_mmcmnc == _p_mmcmnc)
	{
		return record;
	}

	
	if (strcmp(_type, "MOC") == 0)
	{
		record.m_outgoing_calls = _duration;
	}
	else if (strcmp(_type, "MTC") == 0)
	{
		record.m_incoming_calls = _duration;
	}
	else if (strcmp(_type, "SMS-MO") == 0)
	{
		record.m_incoming_sms = 1;
	}
	else if (strcmp(_type, "SMS-MT") == 0)
	{
		record.m_outgoing_sms = 1;
	}

	return record;
}

/************   SubWithin   ************/
static SubRecord  SubWithin(long long _msisdn, char* _type, int _duration, double _upload, double _download)
{
	SubRecord record;
	memset(&record, 0, sizeof(record));

	record.m_msisdn = _msisdn;
	
	if (strcmp(_type, "MOC") == 0)
	{
		record.m_incoming_within_callDuration = _duration;
	}
	else if (strcmp(_type, "MTC") == 0)
	{
		record.m_outgoing_within_callDuration = _duration;
	}
	else if (strcmp(_type, "SMS-MO") == 0)
	{
		record.m_incoming_within_sms = 1;
	}
	else if (strcmp(_type, "SMS-MT") == 0)
	{
		record.m_outgoing_within_sms = 1;
	}
	else if (strcmp(_type, "GPRS") == 0)
	{
		record.m_downloaded = _download;
		record.m_uploaded = _upload;
	}
	
	return record;
}

/************   SubOutside   ************/
static SubRecord  SubOutside(long long _msisdn, char* _type, int _duration, double _upload, double _download)
{
	SubRecord record;
	memset(&record, 0, sizeof(record));
	
	record.m_msisdn = _msisdn;
	
	if (strcmp(_type, "MOC") == 0)
	{
		record.m_incoming_outside_callDuration = _duration;
	}
	else if (strcmp(_type, "MTC") == 0)
	{
		record.m_outgoing_outside_callDuration = _duration;
	}
	else if (strcmp(_type, "SMS-MO") == 0)
	{
		record.m_incoming_outside_sms = 1;
	}
	else if (strcmp(_type, "SMS-MT") == 0)
	{
		record.m_outgoing_outside_sms = 1;
	}
	else if (strcmp(_type, "GPRS") == 0)
	{
		record.m_downloaded = _download;
		record.m_uploaded = _upload;
	}

	return record;
}

/************   SubHandler   ************/
static SubRecord SubHandler(long long _msisdn, int _i_mmcmnc, int _p_mmcmnc, char* _type, int _duration, double _upload, double _download)
{
	if (_i_mmcmnc == _p_mmcmnc)
	{
		return SubWithin(_msisdn, _type, _duration, _upload, _download);
	}

	return SubOutside(_msisdn, _type, _duration, _upload, _download);
}

/************   ParseLine   ************/
int ParseLine(char* line, t_records* msg)
{
	char *token;
	int duration;
	double download;
	double upload;
	long long msisdn;
	int i_mmcmnc;
	char* type;
	int p_mmcmnc;
	
	token = strtok(line, "|"); /* IMSI */
	msisdn = atoll(strtok(NULL, "|"));
	token = strtok(NULL, "|"); /* IMEI */
	token = strtok(NULL, "|");
	i_mmcmnc = atoi(strtok(NULL, "|")); /* my mmc */
	type = strtok(NULL, "|");
	token = strtok(NULL, "|"); /* date */
	token = strtok(NULL, "|"); /* time */
	duration = atoi(strtok(NULL, "|"));
	download = atof(strtok(NULL, "|"));
	upload = atof(strtok(NULL, "|"));
	token = strtok(NULL, "|"); /* party IMSDN */
	token= strtok(NULL, "|");
	p_mmcmnc= atoi(strtok(NULL, "|"));

	*token = EMPTY;
	
	msg->m_operastors   = operatorHandler(i_mmcmnc, p_mmcmnc, type, duration);
	msg->m_subs	= SubHandler(msisdn, i_mmcmnc, p_mmcmnc, type, duration, upload, download);

	return 1;
}

/************   main   ************/
int main(int argc, char* argv[])
{
	FILE* fp;
	char fileName[FILE_SIZE], line[LINE_LENGTH];
	int qId;
	t_records*  msg;
	key_t key;
	pid_t pId = getpid();

	strcpy(fileName,PATHNAME);
	strcat(fileName, argv[1]);
	
	if( NULL == (fp = fopen(fileName,"r")))	
	{
		perror("fopen fail");
		printf("Open file fail");
	}

	printf("Start reading file:%s\n\n",fileName);

	fgets(line, sizeof(line), fp); /* skip first line, which is not a cdr line */

	key = ftok(FILE_NAME, 'a');
	if (-1 == key)
	{
		printf("key fail\n");
	}
	
	(qId = GetQueue(key, pId));/* connect to cdr message queue */
	if (MSG_FAILED == qId)
	{
		return COL_FAIL;
	}

	while(NULL !=( fgets(line, sizeof(line), fp)))
	{	
		msg = (t_records*) calloc (1, sizeof(t_records));
		

		if (COL_SUCCESS != ParseLine(line, msg))
		{
			perror("Parse_Fail"); 
		}

		msg->m_type = RECORD;
		
		if (MSG_FAILED == SendMessage(qId, msg, RECORD, sizeof(t_records)))
		{
			return COL_FAIL;	
		}
	}

	fclose(fp);

}


