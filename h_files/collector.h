#ifndef __COLLECTOR_H__ 
#define __COLLECTOR_H__


/**	@file collector.h 
 *	@author Lev Chakk Morris
 *	@date  
 *	
 *	@breif header file for file collector, which recives CDR files of different formats, and sends speceifed information to the aggregator via a message queue.
 *	
*/


typedef struct t_records t_records;

typedef enum ColRet {
	COL_FAIL	= 0,
	COL_SUCCESS = 1

} ColRet;



/*function  details*/ 


/**  
 * @brief Parse_file recieves a raw data cdr line, packs required information in a t_records structure.
 * @param[in] file - file to be parsed.
 * @param[out] msg - address of t_records struct to be filled.
 * @retval 
 * @warning 
 */
int ParseLine(char* line, t_records* msg);



#endif /* __COLLECTOR_H__ */
