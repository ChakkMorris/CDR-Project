#define _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE

#include<stdio.h>
#include<string.h>
#include <stdlib.h>
#include<errno.h>
#include<sys/msg.h>
#include<sys/inotify.h>
#include<limits.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>



#define BUF_SIZE 200
#define FILE_EXISTS 1
#define NO_FILE 0

#define PATHNAME "../cdr_files"


typedef struct inotify_event inotify_event;



int main()
{

    char* args1[2];
    char* args2[2];
    pid_t CMpid, AGGpid;
	int status;
	
			CMpid = fork();
			args1[0] = "./collector_manager";
			args1[1] = NULL;

			if (CMpid == 0)
			{
				printf("Start CDR manager\n");
				execvp(args1[0],args1);
			}

			AGGpid = fork();
			args2[0] = "./agg";
			args2[1] = NULL;
			
			if (AGGpid == 0)
			{
				printf("Start Aggregator\n");
				execvp(args2[0],args2);
			}
		
			waitpid(AGGpid, &status, 0);
			waitpid(CMpid, &status, 0);
		
}


