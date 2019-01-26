#ifndef __DBMANAGER_H__ 
#define __DBMANAGER_H__

#include "SafeHash.h"
#include "msgStruct.h"

#include <pthread.h>

typedef struct DB 
{
	SafeHash*			m_subHash;
	SafeHash* 			m_operatorHash;
	size_t				m_numThreaDB;
	pthread_mutex_t 	m_subMutex;
	pthread_mutex_t 	m_operatorMutex;
	pthread_mutex_t 	m_flagMutex;  
	pthread_cond_t  	m_runningCond;

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
 * @brief insertToDB - insert cdr information into hash databases.
 * @param[in] msg    - packed message of a single cdr line, to be inserted.
 * @param[in] ds     - Pointer to a structure holding the addresses of both subscriber and operator databases.
 * @retval DB_SUCCESS - when insertion of data successful.
 * @retval DB_ERROR   - when insertion of data unsuccessful.
 * @retval DB_INITIALIZATION_ERR   - when database/s not initialized.
 */ 
retDB insertToDB(t_records* msg, DB* ds);

/**  
 * @brief CreateDatabases        - creats two hash tables, one for operators and a second for subscribers of the client.
 * @param[in] subCapacity		 - capacity of subscriber database.
 * @param[out] operatorCapacity  - capacity of operator database.
 * @return 						 - returns pointer to a DB struct, holding both databases and ipc tools.
 * @retval DB_SUCCESS - when insertion of data successful.
 * @retval DB_ERROR   - when insertion of data unsuccessful. 
 */
DB* CreateDatabases(size_t subCapacity, size_t operatorCapacity);



#endif /* __DBMANAGER_H__ */
