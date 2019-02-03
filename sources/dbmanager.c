#include <stdio.h>
#include <stdlib.h>

#include "msgStruct.h"
#include "dbmanager.h"
#include "SafeHash.h"

#define NUM_THREADS 3 
#define SUB_CAPACITY 6000000
#define OP_CAPACITY 5
#define EQUAL 1
#define NOT_EQUAL 0
#define CLIENT_MMCMNC 42501 /* partner */



/************   equality functions      ************/
int	isEqualSubscriber(const int* _firstKey, const int* _secondKey)
{
	int ret;
	ret = (*(long long*)_firstKey == *(long long*)_secondKey) ? EQUAL : NOT_EQUAL;

	return ret;
}

int	isEqualOperator(void* _firstKey, void* _secondKey)
{
	int ret;
	ret = (*(int*)_firstKey == *(int*)_secondKey) ? EQUAL : NOT_EQUAL;

	return ret;

}

/***********	Hash fucntion	***************/

size_t HashFunc(const void* _key)
{	
	return *(size_t*)_key;	
}



/*********** updateSubHash	*****************/
static retDB updateSubHash(t_records* _msg, SubRecord* _pValue)
{

	_pValue->m_incoming_within_callDuration += _msg->m_subs.m_incoming_within_callDuration;
	_pValue->m_incoming_outside_callDuration += _msg->m_subs.m_incoming_outside_callDuration;
	_pValue->m_outgoing_within_callDuration += _msg->m_subs.m_outgoing_within_callDuration;
	_pValue->m_outgoing_outside_callDuration += _msg->m_subs.m_outgoing_outside_callDuration;
	_pValue->m_incoming_within_sms += _msg->m_subs.m_incoming_within_sms;
	_pValue->m_incoming_outside_sms += _msg->m_subs.m_incoming_outside_sms;
	_pValue->m_outgoing_within_sms += _msg->m_subs.m_outgoing_within_sms;
	_pValue->m_outgoing_outside_sms += _msg->m_subs.m_outgoing_outside_sms;	
	_pValue->m_downloaded += _msg->m_subs.m_downloaded;
	_pValue->m_uploaded += _msg->m_subs.m_uploaded;	


	return DB_SUCCESS;
}


/*********** insertSubscriber	*****************/
static retDB insertSubscriber(t_records* _msg, DB* _hash)
{
	
	int findResult, insertResult;

	SubRecord* pValue ;
	
	findResult = SafeMap_Find(_hash->m_subHash, (void*)&(_msg->m_subs.m_msisdn), (void**)&pValue);

	if (MAP_KEY_EXISTS == findResult)
	{
		
		pthread_mutex_lock(&_hash->m_subMutex);
		updateSubHash(_msg, pValue); 
		pthread_mutex_unlock(&_hash->m_subMutex);
		
		return DB_SUCCESS;
	}
	else if(MAP_KEY_NOT_FOUND == findResult) /* first apearance of key - need to insert */
	{

		insertResult = SafeMap_Insert(_hash->m_subHash, (void*) &(_msg->m_subs.m_msisdn), (void*)&(_msg->m_subs));

		if (insertResult == MAP_SUCCESS)
		{

			return DB_SUCCESS;
		}
		else if (insertResult == MAP_UNINITIALIZED_ERROR)
		{
			return DB_INITIALIZATION_ERR;
		}
		
	}	
	
	pthread_mutex_lock(&_hash->m_subMutex);
	updateSubHash(_msg, pValue); /* if arrived here - other thread has inserted. must update */
	pthread_mutex_unlock(&_hash->m_subMutex);

	
	return DB_SUCCESS;
}

/*********** updateOperatorHash	*****************/
static retDB updateOperatorHash(t_records* _msg, OperatorRecord* _pValue)
{
	_pValue->m_outgoing_calls += _msg->m_operastors.m_outgoing_calls;
	_pValue->m_incoming_calls += _msg->m_operastors.m_incoming_calls;
	_pValue->m_incoming_sms += _msg->m_operastors.m_incoming_sms;
	_pValue->m_outgoing_sms += _msg->m_operastors.m_outgoing_sms;

	return DB_SUCCESS;
}


/*********** insertOperator	*****************/
static retDB insertOperator(t_records* _msg, DB* _hash)
{
	int findResult, insertResult;

	OperatorRecord* pValue;

	findResult = SafeMap_Find(_hash->m_operatorHash, (void*)&_msg->m_operastors.m_mmcmnc, (void**)&pValue);
	
	if (MAP_KEY_EXISTS == findResult)
	{
		pthread_mutex_lock(&_hash->m_operatorMutex);
		updateOperatorHash(_msg, pValue);
		pthread_mutex_unlock(&_hash->m_operatorMutex);
		
		return DB_SUCCESS;
	}
	
	else if(MAP_KEY_NOT_FOUND == findResult) /* first apearance of key - need to insert */	{

		insertResult = SafeMap_Insert(_hash->m_operatorHash, (void*) &_msg->m_operastors.m_mmcmnc, (void*)&_msg->m_operastors);

		if (insertResult == MAP_SUCCESS)
		{
	
			return DB_SUCCESS;
		}
		else if (insertResult == MAP_UNINITIALIZED_ERROR)
		{
			return DB_INITIALIZATION_ERR;
		}
	}	
	 	/* if arrived here, cdr inserted by other thread-> need to update */
	pthread_mutex_lock(&_hash->m_operatorMutex);
	updateOperatorHash(_msg, pValue);
	pthread_mutex_unlock(&_hash->m_operatorMutex);
	
	return DB_SUCCESS;
}

/*********** 	*****************/
	/* API functions */

/*********** 	*****************/


/*********** insertToDB	*****************/
retDB insertToDB(t_records* _msg, DB* _hash)
{
		
	if (DB_SUCCESS != insertSubscriber(_msg, _hash))
	{
		return DB_ERROR;
	}

	if (_msg->m_operastors.m_mmcmnc == CLIENT_MMCMNC || 0 == _msg->m_operastors.m_mmcmnc)
	{
	
		return DB_SUCCESS;
	}

	if (DB_SUCCESS != insertOperator(_msg, _hash))
	{
		return DB_ERROR;
	
	}
	return DB_SUCCESS;
		
}

/*********** CreateDatabases	*****************/
DB* CreateDatabases(size_t _subCapacity, size_t _operatorCapacity)
{
	DB* data;

	data = (DB*) malloc (sizeof(DB));
	
	data->m_subHash =  SafeHash_Create(_subCapacity, HashFunc, (EqualityFunction) isEqualSubscriber, NUM_THREADS); 
	if (NULL == data->m_subHash)
	{
			perror("hash_create fail");
			return NULL;
	}

	data->m_operatorHash =  SafeHash_Create(_subCapacity, HashFunc, (EqualityFunction) isEqualOperator, NUM_THREADS);
	if (NULL == data->m_operatorHash)
	{
			free(data->m_subHash);			
			perror("hash_create fail");
			return NULL;
	}

	if ( 0 != pthread_mutex_init(&(data->m_subMutex), NULL))
	{
		free(data);
		perror("mutex fail");
		return NULL;
	}
	
	if ( 0 != pthread_mutex_init(&(data->m_operatorMutex), NULL))
	{
			
		pthread_mutex_destroy(&data->m_subMutex);
		free(data);
		perror("mutex fail");
		return NULL;
	}
		
	if ( 0 != pthread_mutex_init(&data->m_flagMutex, NULL)) 
	{
		pthread_mutex_destroy(&data->m_operatorMutex);	
		pthread_mutex_destroy(&data->m_subMutex);
		free(data);
		perror("mutex fail");
		return NULL;
	}

	return data;
}
