OBJS= collector_manager.o msgqueue.o 
AGGOBJECTS = msgqueue.o commands.o ../../../advc/ds/hash/safehash.o ../../../advc/ds/hash/list.o ../../../advc/ds/hash/hash_lib.o ../../../advc/ds/hash/list_itr.o  ../../../advc/ds/hash/list_functions.o dbmanager.o


CFLAGS		= -c -pedantic -Wall -Wconversion -g -m32
INCLUDES	= ../incs
HASHINCLUDES = ../../../advc/ds/incs/
LDFLAGS		=
CC		= gcc

############# Testing #########################

hash:
	$(MAKE) -C ../../../advc/ds/hash

all:	
	$(MAKE) collector_manager collector agg ui cdr_generator cdr_procesor 

dbmanager.o: dbmanager.c $(HASHINCLUDES) ../incs/dbmanager.h
	$(CC) $(CFLAGS) -I$(INCLUDES) dbmanager.c -I$(HASHINCLUDES)

collector_manager: $(OBJS)
	$(CC) -o collector_manager $(OBJS) -g -m32

collector: collector.o msgqueue.o 
	$(CC) -o collector -g collector.o msgqueue.o -I$(INCLUDES) -m32

aggregator.o: aggregator.c $(INCLUDES) $(HASHINCLUDES) ../incs/commands.h ../incs/dbmanager.h
	$(CC) $(CFLAGS) -I$(INCLUDES) aggregator.c -I$(HASHINCLUDES)

agg: aggregator.o $(AGGOBJECTS)
	$(CC) -o agg -g aggregator.o $(AGGOBJECTS) -pthread -m32

ui: ui.o msgqueue.o
	$(CC) -o ui -g ui.o msgqueue.o -I$(INCLUDES) -m32

cdr_generator: cdr_generator.o
	$(CC) -o cdr_generator cdr_generator.o -m32

cdr_procesor: cdr_procesor.o
	$(CC) -o cdr_procesor cdr_procesor.o -m32

cdr_procesor.o: cdr_procesor.c 
	$(CC) $(CFLAGS) -I$(INCLUDES) cdr_procesor.c  -m32
	
ui.o: ui.c 
	$(CC) $(CFLAGS) -I$(INCLUDES) ui.c	

msgqueue.o: msgqueue.c 
	$(CC) $(CFLAGS) -I$(INCLUDES) msgqueue.c	

commands.o: commands.c $(INCLUDES) $(HASHINCLUDES) 
	$(CC) $(CFLAGS) -I$(INCLUDES) commands.c -I$(HASHINCLUDES)
	
collector.o: collector.c msgqueue.c ../incs/collector.h ../incs/msgStruct.h
	$(CC) $(CFLAGS) -I$(INCLUDES) collector.c msgqueue.c 

collector_manager.o: collector_manager.c ../incs/collector_manager.h 
	$(CC) $(CFLAGS) -I$(INCLUDES) collector_manager.c 


cdr_generator.o: cdr_generator.c
	$(CC) $(CFLAGS) cdr_generator.c -m32
	


clean:
	rm  *~ *.o ui collector testing cdr_generator agg collect_manager cdr_procesor collector_manager *.out
