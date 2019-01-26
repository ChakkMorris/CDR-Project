#ifndef __COMMANDS_H__ 
#define __COMMANDS_H__


#include "aggregator.h"
#include "SafeHash.h"
#include "msgStruct.h"
#include "dbmanager.h"

/**	@file commands.h
 *	@author Lev Chakk Morris
 *	@date  9.10.17
 *	
 *	@breif Header file for commands module of CDR processor project, containing functions meant for the execution of user commands requested via the program's user interface ("UI");. 
 *	
*/


typedef enum commandRet
{
	COMMAND_SUCCESS,
	COMMAND_FAIL

}commandRet;
 

/**  
 * @brief OperatorQuary    - This functions retrieves the up-to-date aggregated inter-operator settlement data with a specified cellular operator, e.i. phone call durations and SMS messages sent/recieved. 
 * @param[in] agg	   - Pointer to the databases, which contain the target's information.
 * @param[in] msg	   - Message recieved from UI, contaning the target of query.
 * @param[out] returnMsg   - created by the command thread in the "aggregator" module, which will be sent back to the UI, with the quary results. If 				     oprator is meassing from DB, UI will recieve an according message.
 * @retval COMMAND_SUCCESS - If query executed successfully.
 * @retval COMMAND_FAIL	   - If query unsuccessful.
 */
commandRet OperatorQuery(DB* hash, command* msg, command* returnMsg);


/**  
 * @brief SubscriberQuary - This functions retrieves the up-to-date aggregated billing data of a specified  subscriber of the client operator.   
 * @param[in] agg	   - Pointer to the databases, which contain the target's information.
  * @param[in] msg	   - Message recieved from UI, contaning the target of query.
 * @param[out] returnMsg   - Pointer to a message, created by the command thread in the "aggregator" module, which will be sent back to the the UI, with 				   the quary results. If oprator is meassing from DB, UI will recieve an according message.
 * @retval COMMAND_SUCCESS - If quary executed successfully.
 * @retval COMMAND_FAIL	   - If query unsuccessful.
 */
commandRet SubscriberQuery(DB* hash, command* _msg, command* _returnMsg);

/**  
 * @brief SubReport  - Function creats an up-to-date text file report with aggregated information of all subscribers.
 * @param[in] agg	   - Pointer to the databases, which contain all subscriber information.
 * @param[in] msg	   - Message recieved from UI, contaning the target of query.
 * @param[out] returnMsg   - Pointer to a message, created by the command thread in the "aggregator" module, which will be sent back to the the UI, informing user of creation of file when succesful, or error message when unsuccessfull.
 * @retval COMMAND_SUCCESS - If quary executed successfully.
 * @retval COMMAND_FAIL	   - If query unsuccessful.
 */
commandRet SubReport(DB* hash, command* _returnMsg);

/**  
 * @brief SubReport  - Function creats an up-to-date text file report with aggregated information of all subscribers.
 * @param[in] agg	   - Pointer to the databases, which contain all subscriber information.
 * @param[in] msg	   - Message recieved from UI, contaning the target of report.
 * @param[out] returnMsg   - Pointer to a message, created by the command thread in the "aggregator" module, which will be sent back to the the UI, informing user of creation of file when succesful, or error message when unsuccessfull.
 * @retval COMMAND_SUCCESS - If quary executed successfully.
 * @retval COMMAND_FAIL	   - If query unsuccessful.
 */
commandRet OperatorReport(DB* hash, command* _returnMsg);

/**  
 * @brief PuaseProcessor - Pause work of processor if running.  
 * @param[in] agg	  - Pointer to aggregator structure.
 * @param[out] returnMsg  -  Pointer to a message, created by the command thread in the "aggregator" module, which will be sent back to the the UI, informing that processor is currently paused.
 * @retval COMMAND_SUCCESS.
 */
commandRet PuaseProcessor(Aggregator* agg, command* _returnMsg);

/**  
 * @brief ResumeProcessor - Resume work of processor when paused.  
 * @param[in] agg	  - Pointer to aggregator structure.
 * @param[out] returnMsg  -  Pointer to a message, created by the command thread in the "aggregator" module, which will be sent back to the the UI, informing that processor is running.
 * @retval COMMAND_SUCCESS.
 */
commandRet ResumeProcessor(Aggregator* agg, command* _returnMsg);

/**  
 * @brief Shutdown - Finishes work of CDR processor.  
 * @param[in] msg	   - Message recieved from UI, contaning the target of report.
 * @param[out] returnMsg   - Pointer to a message, created by the command thread in the "aggregator" module, which will be sent back to the the UI, informing that processor has been shut down if succesful.
 * @retval COMMAND_SUCCESS - If quary executed successfully.
 * @retval COMMAND_FAIL	   - If query unsuccessful.
 */
commandRet Shutdown(Aggregator* agg, command* _returnMsg);

/**  
 * @brief setSignals - Sets signal handlers for SIGUSER_1 and SIGUSER_2, which create a subsciber report and operator report ( respectively) when signaled.  
 * @retval COMMAND_SUCCESS - If quary executed successfully.
 * @retval COMMAND_FAIL	   - If query unsuccessful.
 */
commandRet setSignals();





#endif /* __COMMANDS_H__ */
