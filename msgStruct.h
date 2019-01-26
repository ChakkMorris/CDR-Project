#ifndef __MSGSTRUCT_H__ 
#define __MSGSTRUCT_H__

#define LENGTH 1000
#define TWO 2

/**	@file msgStruct.h
 *	@author lev chakk morris
 *	@date  
 *	
 *	@breif Header file for messages sent via the message queues
 *	
*/



typedef struct SubRecord
{
	long long		m_msisdn;
	long long 		m_incoming_within_callDuration;
	long long		m_incoming_outside_callDuration;
	long long 		m_outgoing_within_callDuration;
	long long 		m_outgoing_outside_callDuration;
	long long 		m_incoming_within_sms;
	long long		m_incoming_outside_sms;
	long long 		m_outgoing_within_sms;
	long long 		m_outgoing_outside_sms;		
	double 		m_downloaded;
	double		m_uploaded;
}SubRecord;


typedef struct OperatorRecord
{
	int 		m_mmcmnc;
	long long 	m_outgoing_calls;
	long long 	m_incoming_calls;	
	long long 	m_incoming_sms;
	long long 	m_outgoing_sms;

}OperatorRecord;



typedef struct t_records
{	
	long 			m_type;
	SubRecord		m_subs;
	OperatorRecord  m_operastors;

}t_records;



typedef struct command
{	
	long 	  	m_type;
	int 	  	m_command;
	int	  	  	m_mmcmnc;
	long long	m_msisdn;
	char	  	m_returnMessage[LENGTH];
}command;





#endif /* __MSGSTRUCT_H__ */
