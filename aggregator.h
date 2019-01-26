#ifndef __AGGREGATOR_H__ 
#define __AGGREGATOR_H__
#include <pthread.h>
#include <sys/types.h>
#include <semaphore.h>

#include "dbmanager.h"

/**	@file aggregator.h
 *	@author Lev Chakk Morris
 *	@date  
 *	
 *	@breif Calls for the creation of the DBmanager, creats the command handler, which handles UI commands, and creates a predefiened number of message 	   handlers which recieve incoming CDR information messages via CDR message queue, sending them to DBmanager for database updating.
 *	
*/


typedef struct Aggregator
{

	DB*					m_DBmanager;
	size_t				m_numThreads;
	pthread_mutex_t 	m_flagMutex;  
	pthread_cond_t  	m_runningCond;
	sem_t				m_shutdownSem;
	size_t 				m_runningFlag;
	size_t 				m_aggregatorCounter;
	int					m_readerMsgQueue;
	int					m_commandMsgQueue;
	int* 				m_sharedMem;
}Aggregator;		



typedef enum retAgg
{
	AGG_SUCCESS,
	AGG_INITIALIZATION_ERR,
	AGG_ERROR,
	AGG_INSERTED_TO_HASH,
	MAP_UPDATED
}retAgg;


/*typedef size_t (*HashFunction)(const void* _key);*/

/*function  details*/ 

/**  
 * @brief createAggregator - Creates the Aggregator structure with memory allocation. 
 * @param[in] _numThreads  - Number of message hundler threads to be created for DR processing purposes.
 * @param[in] operatorCapacity - Capacity of subscirber database the be created.
 * @param[in]	subCapacity -  Capacity of subscirber database the be created.
 * @return 	- Return pointer to aggregator structure.	
 * @retval 
 */
Aggregator* createAggregator(size_t _numThreads, size_t subCapacity, size_t operatorCapacity);




#endif /* __AGGREGATOR_H__ */
