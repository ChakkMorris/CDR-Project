#ifndef _XOPEN_SOURCE 
#define _XOPEN_SOURCE 500
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define GENERATOR_ERROR -1
#define GENERATOR_SUCCESS 0

#define LINES 8000000
#define MSISDN 408700000000000
#define IMSI   208700000000000
#define LIMIT 1000
#define TUPLE  42501
#define T_LIMIT 6
#define D_LIMIT 10000
#define G_LIMIT 9999

#define PATHNAME "../cdr_files/"

typedef void (*Generator)(FILE*);

void GetOpName(char* _name, int _tuple)
{
	switch(_tuple)
	{
		case 42501:
			strcpy(_name, "Partner");
			return;
		case 42502:
			strcpy(_name, "Cellcom");
			return;	
		case 42503:
			strcpy(_name, "Pelephone");
			return;
		case 42504:
			strcpy(_name, "HotMobile");
			return;
		case 42505:
			strcpy(_name, "YouPhone");
			return;
		case 42506:
			strcpy(_name, "GolanTelecom");
			return;				
		case 42507:
			strcpy(_name, "Bezeq");
			return;		
	}
}

void CDR_genrateMOC(FILE* _fp)
{
	long long imsi, msisdn, msisdnP = 0;
	int pTuple, dur;
	char pOp[64], imei[30];
	char dateTime[100];
	struct tm* now_tm;
	time_t now;
	time(&now);

	now_tm = localtime(&now);
	strftime(dateTime, 100, "%d/%m/%Y|%H:%M:%S", now_tm);
	
	imsi = IMSI + rand() % LIMIT;
	msisdn = MSISDN + rand() % LIMIT;
	pTuple = TUPLE + rand() % T_LIMIT;
	dur = rand() % D_LIMIT;
	while(msisdnP == msisdn)
	{
		msisdnP = MSISDN + rand() % LIMIT;
	}
	
	GetOpName(pOp, pTuple);
	snprintf(imei, 30, "%d-%d-%d-%d", 10 + rand() % 89, 10000 + rand() % 89999, 1000 + rand() % 8999, rand() % 9);

	fprintf(_fp, "%llu|%llu|%s|Partner|42501|MOC|%s|%d|0.00|0.00|%llu|%s|%d\n", 
		imsi, msisdn, imei, dateTime, dur, msisdnP, pOp, pTuple);
}

void CDR_genrateMTC(FILE* _fp)
{
	long long imsi, msisdn, msisdnP = 0;
	int pTuple, dur;
	char pOp[64], imei[30], dateTime[100];
	struct tm* now_tm;
	time_t now;
	time(&now);

	now_tm = localtime(&now);
	strftime(dateTime, 100, "%d/%m/%Y|%H:%M:%S", now_tm);
	
	imsi = IMSI + rand() % LIMIT;
	msisdn = MSISDN + rand() % LIMIT;
	pTuple = TUPLE + rand() % T_LIMIT;
	dur = rand() % D_LIMIT;
	while(msisdnP != msisdn)
	{
		msisdnP = MSISDN + rand() % LIMIT;
	}
	
	GetOpName(pOp, pTuple);
	snprintf(imei, 30, "%d-%d-%d-%d", 10 + rand() % 89, 10000 + rand() % 89999, 1000 + rand() % 8999, rand() % 9);

	fprintf(_fp, "%llu|%llu|%s|Partner|42501|MTC|%s|%d|0.00|0.00|%llu|%s|%d\n", 
		imsi, msisdn, imei, dateTime, dur, msisdnP, pOp, pTuple);
}

void CDR_genrateSMS_MO(FILE* _fp)
{
	long long imsi, msisdn, msisdnP = 0;
	int pTuple;
	char pOp[64], imei[30];
	char dateTime[100];
	struct tm* now_tm;
	time_t now;
	time(&now);

	now_tm = localtime(&now);
	strftime(dateTime, 100, "%d/%m/%Y|%H:%M:%S", now_tm);
	
	imsi = IMSI + rand() % LIMIT;
	msisdn = MSISDN + rand() % LIMIT;
	pTuple = TUPLE + rand() % T_LIMIT;
	while(msisdnP != msisdn)
	{
		msisdnP = MSISDN + rand() % LIMIT;
	}
	
	GetOpName(pOp, pTuple);
	snprintf(imei, 30, "%d-%d-%d-%d", 10 + rand() % 89, 10000 + rand() % 89999, 1000 + rand() % 8999, rand() % 9);

	fprintf(_fp, "%llu|%llu|%s|Partner|42501|SMS-MO|%s|0|0.00|0.00|%llu|%s|%d\n", 
		imsi, msisdn, imei, dateTime, msisdnP, pOp, pTuple);
}

void CDR_genrateSMS_MT(FILE* _fp)
{
	long long imsi, msisdn, msisdnP = 0;
	int pTuple;
	char pOp[64], imei[30];
	char dateTime[100];
	struct tm* now_tm;
	time_t now;
	time(&now);

	now_tm = localtime(&now);
	strftime(dateTime, 100, "%d/%m/%Y|%H:%M:%S", now_tm);
	
	imsi = IMSI + rand() % LIMIT;
	msisdn = MSISDN + rand() % LIMIT;
	pTuple = TUPLE + rand() % T_LIMIT;
	while(msisdnP != msisdn)
	{
		msisdnP = MSISDN + rand() % LIMIT;
	}
	
	GetOpName(pOp, pTuple);
	snprintf(imei, 30, "%d-%d-%d-%d", 10 + rand() % 89, 10000 + rand() % 89999, 1000 + rand() % 8999, rand() % 9);

	fprintf(_fp, "%llu|%llu|%s|Partner|42501|SMS-MT|%s|0|0.00|0.00|%llu|%s|%d\n", 
		imsi, msisdn, imei, dateTime, msisdnP, pOp, pTuple);
}


void CDR_genrateGPRS_Down(FILE* _fp)
{
	long long imsi, msisdn, msisdnP = 0;
	int pTuple, dur;
	char pOp[64], imei[30];
	char dateTime[100];
	struct tm* now_tm;
	time_t now;
	time(&now);

	now_tm = localtime(&now);
	strftime(dateTime, 100, "%d/%m/%Y|%H:%M:%S", now_tm);
	
	imsi = IMSI + rand() % LIMIT;
	msisdn = MSISDN + rand() % LIMIT;
	pTuple = TUPLE + rand() % T_LIMIT;
	dur = rand() % G_LIMIT;
	while(msisdnP != msisdn)
	{
		msisdnP = MSISDN + rand() % LIMIT;
	}
	
	GetOpName(pOp, pTuple);
	snprintf(imei, 30, "%d-%d-%d-%d", 10 + rand() % 89, 10000 + rand() % 89999, 1000 + rand() % 8999, rand() % 9);

	fprintf(_fp, "%llu|%llu|%s|Partner|42501|GPRS|%s|0|%2.2d.%2.2d|0.00|%llu|%s|%d\n", 
		imsi, msisdn, imei, dateTime, dur / 100, dur % 100, msisdnP, pOp, pTuple);
}

void CDR_genrateGPRS_Up(FILE* _fp)
{
	long long imsi, msisdn, msisdnP = 0;
	int pTuple, dur;
	char pOp[64], imei[30];
	char dateTime[100];
	struct tm* now_tm;
	time_t now;
	time(&now);

	now_tm = localtime(&now);
	strftime(dateTime, 100, "%d/%m/%Y|%H:%M:%S", now_tm);
	
	imsi = IMSI + rand() % LIMIT;
	msisdn = MSISDN + rand() % LIMIT;
	pTuple = TUPLE + rand() % T_LIMIT;
	dur = rand() % G_LIMIT;
	while(msisdnP != msisdn)
	{
		msisdnP = MSISDN + rand() % LIMIT;
	}
	
	GetOpName(pOp, pTuple);
	snprintf(imei, 30, "%d-%d-%d-%d", 10 + rand() % 89, 10000 + rand() % 89999, 1000 + rand() % 8999, rand() % 9);

	fprintf(_fp, "%llu|%llu|%s|Partner|42501|GPRS|%s|0|0.00|%2.2d.%2.2d|%llu|%s|%d\n", 
		imsi, msisdn, imei, dateTime, dur / 100, dur % 100, msisdnP, pOp, pTuple);
}


int main(int argc, char* argv[])
{
	int i = 0, fileNum = 0, j = 0, count = 0, max;
	char fileName[256], filePath[1024], command[1024];	
	FILE* fPtr;
	Generator genFunc[] = {CDR_genrateMOC , CDR_genrateMTC, CDR_genrateSMS_MO, CDR_genrateSMS_MT, CDR_genrateGPRS_Up, CDR_genrateGPRS_Down};
	
	
	max = 2 + rand() % 3;
	strcpy(filePath, PATHNAME);
	
	for(;;)
	{
		snprintf(fileName, 256, "./cdr%d.cdr", fileNum++);

		fPtr = fopen(fileName, "w+");
		if (NULL == fPtr)
		{
			return GENERATOR_ERROR;
		}
		
		fprintf(fPtr, "%d\n", LINES);
		for(i = 0; i < LINES; ++i)
		{
			j = rand() % 6;
			genFunc[j](fPtr);		
		}
		
		fclose(fPtr);
		
		snprintf(command, 1024, "mv %s %s", fileName, filePath);
		system(command);
		
		printf("%s created succesfully in %s!\n", fileName, filePath);
		++count;		
		
		if (count == max)
		{
			max = 2 + rand() % 3;
			count = 0;
			sleep(100);			
		}
	}
	
	return GENERATOR_SUCCESS;

}
