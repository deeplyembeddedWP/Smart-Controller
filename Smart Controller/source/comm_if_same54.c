/*
 * comm_if_same54.c
 *
 * Created: 1/12/2021 11:08:33 AM
 *  Author: Developer
 */ 

/* Atmel drivers for serial port access */
#include <atmel_start.h>
#include <string.h>

/* Platform layer includes. */
#include "cellular_platform.h"

/* Cellular comm interface include file. */
#include "cellular_config.h"
#include "cellular_config_defaults.h"
#include "cellular_comm_interface.h"

/*-----------------------------------------------------------*/
/* Comm status. */
#define CELLULAR_COMM_OPEN_BIT               ( 0x01U )

/* Comm task event. */
#define COMMTASK_EVT_MASK_STARTED            ( 0x0001UL )
#define COMMTASK_EVT_MASK_ABORT              ( 0x0002UL )
#define COMMTASK_EVT_MASK_ABORTED            ( 0x0004UL )
#define COMMTASK_EVT_MASK_ALL_EVENTS \
( COMMTASK_EVT_MASK_STARTED      \
| COMMTASK_EVT_MASK_ABORT      \
| COMMTASK_EVT_MASK_ABORTED )
#define COMMTASK_POLLING_TIME_MS             ( 1UL )

/* Platform thread stack size and priority. */
#define COMM_IF_THREAD_DEFAULT_STACK_SIZE    ( 2048U )
#define COMM_IF_THREAD_DEFAULT_PRIORITY      ( tskIDLE_PRIORITY + 5U )


/*-----------------------------------------------------------*/
typedef struct _cellularCommContext
{
	CellularCommInterfaceReceiveCallback_t commReceiveCallback;
	TaskHandle_t commReceiveCallbackThread;
	uint8_t commStatus;
	void * pUserData;
	CellularCommInterface_t * pCommInterface;
	bool commTaskThreadStarted;
	EventGroupHandle_t pCommTaskEvent;
} _cellularCommContext_t;

/*-----------------------------------------------------------*/

/**
 * @brief CellularCommInterfaceOpen_t implementation.
 */
static CellularCommInterfaceError_t _prvCommIntfOpen( CellularCommInterfaceReceiveCallback_t receiveCallback,
                                                      void * pUserData,
                                                      CellularCommInterfaceHandle_t * pCommInterfaceHandle );

/**
 * @brief CellularCommInterfaceSend_t implementation.
 */
static CellularCommInterfaceError_t _prvCommIntfSend( CellularCommInterfaceHandle_t commInterfaceHandle,
                                                      const uint8_t * pData,
                                                      uint32_t dataLength,
                                                      uint32_t timeoutMilliseconds,
                                                      uint32_t * pDataSentLength );

/**
 * @brief CellularCommInterfaceRecv_t implementation.
 */
static CellularCommInterfaceError_t _prvCommIntfReceive( CellularCommInterfaceHandle_t commInterfaceHandle,
                                                         uint8_t * pBuffer,
                                                         uint32_t bufferLength,
                                                         uint32_t timeoutMilliseconds,
                                                         uint32_t * pDataReceivedLength );

/**
 * @brief CellularCommInterfaceClose_t implementation.
 */
static CellularCommInterfaceError_t _prvCommIntfClose( CellularCommInterfaceHandle_t commInterfaceHandle );


/**
 * @brief Get default comm interface context.
 *
 * @return On success, SOCKETS_ERROR_NONE is returned. If an error occurred, error code defined
 * in sockets_wrapper.h is returned.
 */
static _cellularCommContext_t * _getCellularCommContext( void );


/*-----------------------------------------------------------*/
CellularCommInterface_t CellularCommInterface =
{
	.open  = _prvCommIntfOpen,
	.send  = _prvCommIntfSend,
	.recv  = _prvCommIntfReceive,
	.close = _prvCommIntfClose
};

static _cellularCommContext_t _iotCellularCommContext =
{
	.pCommInterface            = &CellularCommInterface,
	.pUserData                 = NULL,
	.commStatus                = 0U,
	.pCommTaskEvent            = NULL
};
/*-----------------------------------------------------------*/

static _cellularCommContext_t * _getCellularCommContext( void ){
	return &_iotCellularCommContext;
}

/*-----------------------------------------------------------*/

static CellularCommInterfaceError_t _prvCommIntfOpen( CellularCommInterfaceReceiveCallback_t receiveCallback, void * pUserData,
														CellularCommInterfaceHandle_t * pCommInterfaceHandle ){
															
	CellularCommInterfaceError_t commIntRet = IOT_COMM_INTERFACE_SUCCESS;
	_cellularCommContext_t * pCellularCommContext = _getCellularCommContext(); 
	
	if( pCellularCommContext == NULL ){
		   commIntRet = IOT_COMM_INTERFACE_FAILURE;
	}else if( ( pCellularCommContext->commStatus & CELLULAR_COMM_OPEN_BIT ) != 0 ){
		   CellularLogError( "Cellular comm interface opened already" );
		   commIntRet = IOT_COMM_INTERFACE_FAILURE;
	}else{
		memset( pCellularCommContext, 0, sizeof( _cellularCommContext_t ) );
		pCellularCommContext->pCommInterface = &CellularCommInterface;
	}
	
   if( commIntRet == IOT_COMM_INTERFACE_SUCCESS ){
	   pCellularCommContext->commReceiveCallback = receiveCallback;
	}
   
  if( commIntRet == IOT_COMM_INTERFACE_SUCCESS ){
	  pCellularCommContext->pUserData = pUserData;
	  *pCommInterfaceHandle = ( CellularCommInterfaceHandle_t ) pCellularCommContext;
	  pCellularCommContext->commStatus |= CELLULAR_COMM_OPEN_BIT;
	}

	return commIntRet;
}


static CellularCommInterfaceError_t _prvCommIntfSend( CellularCommInterfaceHandle_t commInterfaceHandle,
													  const uint8_t * pData, uint32_t dataLength,
													  uint32_t timeoutMilliseconds, uint32_t * pDataSentLength ){
	CellularCommInterfaceError_t commIntRet = IOT_COMM_INTERFACE_SUCCESS;
	_cellularCommContext_t * pCellularCommContext = ( _cellularCommContext_t * ) commInterfaceHandle;
	
    if( pCellularCommContext == NULL ){
	    commIntRet = IOT_COMM_INTERFACE_BAD_PARAMETER;
    }else if( ( pCellularCommContext->commStatus & CELLULAR_COMM_OPEN_BIT ) == 0 ){
	    CellularLogError( "Cellular send comm interface is not opened before." );
	    commIntRet = IOT_COMM_INTERFACE_FAILURE;
    } else{
		struct io_descriptor *io;
		usart_os_get_io(&SERCOM_1_UART, &io);
		
		int32_t dwWritten = io->write(io,  pData, dataLength);
		if(dwWritten < 0){
			CellularLogError( "Cellular Write fail %d",dwWritten);
			commIntRet = IOT_COMM_INTERFACE_FAILURE;
		}else{
			*pDataSentLength = (uint32_t)dwWritten;
		}			
	}
	
	return commIntRet;
}

static CellularCommInterfaceError_t _prvCommIntfReceive( CellularCommInterfaceHandle_t commInterfaceHandle,
														 uint8_t * pBuffer, uint32_t bufferLength,
														 uint32_t timeoutMilliseconds, uint32_t * pDataReceivedLength ){
	CellularCommInterfaceError_t commIntRet = IOT_COMM_INTERFACE_SUCCESS;
    _cellularCommContext_t * pCellularCommContext = ( _cellularCommContext_t * ) commInterfaceHandle;
	
    if( pCellularCommContext == NULL ){
	    commIntRet = IOT_COMM_INTERFACE_BAD_PARAMETER;
    }else if( ( pCellularCommContext->commStatus & CELLULAR_COMM_OPEN_BIT ) == 0 ){
	    CellularLogError( "Cellular read comm interface is not opened before." );
	    commIntRet = IOT_COMM_INTERFACE_FAILURE;
    }
	
	if( commIntRet == IOT_COMM_INTERFACE_SUCCESS ){
		struct io_descriptor *io;
		usart_os_get_io(&SERCOM_1_UART, &io);
		
		int32_t dwRead = io->read(io, pBuffer, bufferLength);
		if(dwRead < 0){
            CellularLogError( "Cellular ReadFile fail %d", dwRead );
            commIntRet = IOT_COMM_INTERFACE_FAILURE;
		}else{
			*pDataReceivedLength = ( uint32_t ) dwRead;
		}
	}
		
	return commIntRet;
}

static CellularCommInterfaceError_t _prvCommIntfClose( CellularCommInterfaceHandle_t commInterfaceHandle ){
	CellularCommInterfaceError_t commIntRet = IOT_COMM_INTERFACE_SUCCESS;
	
	return commIntRet;
	
}
/*-----------------------------------------------------------*/

