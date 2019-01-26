#ifndef __MSGQUEUE_H__ 
#define __MSGQUEUE_H__


#include "msgStruct.h"

/**	@file msgqueue.h
 *	@author lev chakk morris
 *	@date  
 *	
 *	@breif Header file for use of an unnamed message queue of generic messages for CDR prject 
 *	
*/

typedef enum msgRet
{
	MSG_FAILED = -1,
	MSG_SUCCESS,
	QUEUE_TREM
}msgRet;

/**  
 * @brief GetQueue	- Get queue (or create if not created) to calling proccess;
 * @param[in]- _key	- Key needed to connect to specific queue.
 * @param[in]- _pId	- Id of calling proccess.
 * @return 		- Retruns identification number of queue.
 * @warning 		- Key should be created with ftok() function. 
 */
int GetQueue(key_t _key, pid_t _pId);

/**  
 * @brief SendMessage   - Send message through queue created/joined in GetQueue function.
 * @param[in] qId	- Identification number of queue.
 * @param[in] msg	- Pointer to specific message to be sent.
 * @param[in] type	- Type of message sent.
 * @retval  MSG_SUCCESS - When message sent successfully.
 * @retval  MSG_FAILED  - When SendMessage was unsuccessfull.
 */
 msgRet SendMessage(int qId, void* msg, int type, size_t size);
 
 /**  
 * @brief RecieveMessage- recieve message via queue created/joined in GetQueue function.
 * @param[in] qId	- Identification number of queue.
 * @param[in] msg	- Pointer to struct where message will be stored.
 * @param[in] type	- Type of message sent.
 * @retval  MSG_SUCCESS - When message sent successfully.
 * @retval  MSG_FAILED  - When SendMessage was unsuccessfull.
 */
 msgRet RecieveMessage(int qId, void* msg, int type, size_t size);
 
  /**  
 * @brief ExitQueue   - destroy message queue provided.
 * @param[in] qId	- Identification number of queue.
 * @retval  MSG_SUCCESS - When message sent successfully.
 * @retval  MSG_FAILED  - When SendMessage was unsuccessfull.
 */
 msgRet ExitQueue(int _qId); 
  /**  
 * @brief getSharedMemory - connects process to a shared memory segment.   
 * @param[in] key	      - Key to connection, provided with ftok function.
 * @return 				  - Pointer to shared memory.
 */
int* getSharedMemory(key_t key);



 
#endif /* __MSGQUEUE_H__ */
