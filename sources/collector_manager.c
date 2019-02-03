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

#include "collector_manager.h"
#include "msgqueue.h"

#define BUF_SIZE 200
#define FILE_EXISTS 1
#define NO_FILE 0

#define PATHNAME "../cdr_files"
#define FILE_NAME "./file.txt"
#define NEWPATH "../archive"
typedef struct inotify_event inotify_event;

int main()
{
    int instance, watchEvent;
    char buffer[BUF_SIZE];
    ssize_t lengthRead;
    char *p;
    inotify_event* event_info;
    char* args[3];
    pid_t cpid;
	int* shutDownFlag;
	key_t key;

    instance = inotify_init();
    if(instance == -1)
    {
  		perror("init fail");
       	exit(1); 
    }

	key = ftok(FILE_NAME, 'b');
	if (-1 == key)
	{
		perror("Key_Fail");
	}

	shutDownFlag = getSharedMemory(key);
	*shutDownFlag = 0;

	watchEvent = inotify_add_watch(instance, PATHNAME, IN_CREATE | IN_MOVED_TO);
    if(watchEvent == -1)
    {
  		perror("inotify_watch fail");
       	exit(1); 
    }
		
	for(;;) /* infinite loop which creates collector process for incoming cdr files */
	{

		if (1 == *shutDownFlag) /* stop sending files to queue */
		{
			*shutDownFlag = 2;
			break;
		}

		lengthRead = read(instance, buffer, BUF_SIZE);
		if(lengthRead < 1)
		{
	  		perror("read fail");
		   	return NO_FILE;
		}
		
		for(p = buffer; p < buffer + lengthRead; p += sizeof(inotify_event) + event_info->len)
	   	{
			cpid = fork();
			event_info = (inotify_event *) p;
			args[0] = "./collector";
			args[1] = event_info->name;
			args[2] = NULL;

			if (cpid == 0)
			{
				printf("open new collector\n");
				execvp(args[0],args);
			}
		}
	}		
}


