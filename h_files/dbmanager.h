#ifndef __DBMANAGER_H__ 
#define __DBMANAGER_H__

#include "SafeHash.h"
#include "msgStruct.h"

#include <pthread.h>

typedef struct DB /* TODO update */
{
	SafeHash*			m_subHash;
	SafeHash* 			m_operatorHash;
	size_t				m_numThreaDB;
	pthread_mutex_t 	m_subMutex;
	pthread_mutex_t 	m_operatorMutex;
	pthread_mutex_t 	m_flagMutex;  
	pthread_cond_t  	m_runningCond;
	size_t 				m_runningFlag;
	size_t 				m_aggregatorCounter;
	size_t				m_shutdownFlag;
	int					m_readerMsgQueue;
	int					m_commandMsgQueue;

}DB;		



typedef enum retDB
{
	DB_SUCCESS,
	DB_INITIALIZATION_ERR,
	DB_ERROR,
	DB_INSERTED_TO_HASH,

}retDB;


/**	@file DBmanager.h
 *	@author Lev Chakk Morris
 *	@date  10.8.17
 *	
 *	@breif explenation Header file for DBmanager module, which is in charge of dealing with the hash databases used by the cdr processor. 
 *	
*/



/*function  details*/ 

/**  
 * @brief insertToDB
 * @param[in] 
 * @param[out]
 * @return 
 * @retval 
 * @warning 
 */
retDB insertToDB(t_records* msg, DB* ds);


/**  
 * @brief insertToDB
 * @param[in] 
 * @param[out]
 * @return 
 * @retval 
 * @warning 
 */
DB* CreateDatabases(size_t _subCapacity, size_t _operatorCapacity);











#endif /* __DBMANAGER_H__ */
