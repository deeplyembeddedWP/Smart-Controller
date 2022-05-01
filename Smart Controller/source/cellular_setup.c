/*
 * cellular_setup.c
 *
 * Created: 26/11/2021 1:57:38 PM
 *  Author: Developer
 */

/**
 * @file cellular_setup.c
 * @brief Setup cellular connectivity for board with cellular module.
 */

/* FreeRTOS include. */
#include <FreeRTOS.h>
#include "task.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

/* FreeRTOS Cellular Library include. */
#include "cellular_config.h"
#include "cellular_config_defaults.h"
#include "cellular_types.h"
#include "cellular_api.h"
#include "cellular_comm_interface.h"

/*-----------------------------------------------------------*/

#ifndef CELLULAR_APN
    #error "CELLULAR_APN is not defined in cellular_config.h"
#endif

#define CELLULAR_SIM_CARD_WAIT_INTERVAL_MS       ( 500UL )
#define CELLULAR_MAX_SIM_RETRY                   ( 5U )

#define CELLULAR_PDN_CONNECT_WAIT_INTERVAL_MS    ( 1000UL )

/*-----------------------------------------------------------*/

/* the default Cellular comm interface in system. */
extern CellularCommInterface_t CellularCommInterface;

/*-----------------------------------------------------------*/

/* Secure socket needs application to provide the cellular handle and pdn context id. */
/* User of secure sockets cellular should provide this variable. */
CellularHandle_t CellularHandle = NULL;

/* User of secure sockets cellular should provide this variable. */
uint8_t CellularSocketPdnContextId = CELLULAR_PDN_CONTEXT_ID;

/*-----------------------------------------------------------*/

bool setupCellular( void )
{
    bool cellularRet = true;
    CellularError_t cellularStatus = CELLULAR_SUCCESS;
    CellularSimCardStatus_t simStatus = { 0 };
    CellularServiceStatus_t serviceStatus = { 0 };
    CellularCommInterface_t * pCommIntf = &CellularCommInterface;
    uint8_t tries = 0;
    CellularPdnConfig_t pdnConfig = { CELLULAR_PDN_CONTEXT_IPV4, CELLULAR_PDN_AUTH_NONE, CELLULAR_APN, "", "" };
    CellularPdnStatus_t PdnStatusBuffers = { 0 };
    char localIP[ CELLULAR_IP_ADDRESS_MAX_SIZE ] = { '\0' };
    uint32_t timeoutCountLimit = ( CELLULAR_PDN_CONNECT_TIMEOUT / CELLULAR_PDN_CONNECT_WAIT_INTERVAL_MS ) + 1U;
    uint32_t timeoutCount = 0;
    uint8_t NumStatus = 1;

    /* Initialize Cellular Comm Interface. */
    cellularStatus = Cellular_Init( &CellularHandle, pCommIntf );

    if( cellularStatus == CELLULAR_SUCCESS )
    {
        /* wait until SIM is ready */
        for( tries = 0; tries < CELLULAR_MAX_SIM_RETRY; tries++ )
        {
            cellularStatus = Cellular_GetSimCardStatus( CellularHandle, &simStatus );

            if( ( cellularStatus == CELLULAR_SUCCESS ) &&
                ( ( simStatus.simCardState == CELLULAR_SIM_CARD_INSERTED ) &&
                  ( simStatus.simCardLockState == CELLULAR_SIM_CARD_READY ) ) )
            {
                configPRINTF( ( ">>>  Cellular SIM okay  <<<\r\n" ) );
                break;
            }
            else
            {
                configPRINTF( ( ">>>  Cellular SIM card state %d, Lock State %d <<<\r\n",
                                simStatus.simCardState,
                                simStatus.simCardLockState ) );
            }

            vTaskDelay( pdMS_TO_TICKS( CELLULAR_SIM_CARD_WAIT_INTERVAL_MS ) );
        }
    }

    /* Setup the PDN config. */
    if( cellularStatus == CELLULAR_SUCCESS )
    {
        cellularStatus = Cellular_SetPdnConfig( CellularHandle, CellularSocketPdnContextId, &pdnConfig );
    }
    else
    {
        configPRINTF( ( ">>>  Cellular SIM failure  <<<\r\n" ) );
    }

    /* Rescan network. */
    if( cellularStatus == CELLULAR_SUCCESS )
    {
        cellularStatus = Cellular_RfOff( CellularHandle );
    }

    if( cellularStatus == CELLULAR_SUCCESS )
    {
        cellularStatus = Cellular_RfOn( CellularHandle );
    }

    /* Get service status. */
    if( cellularStatus == CELLULAR_SUCCESS )
    {
        while( timeoutCount < timeoutCountLimit )
        {
            cellularStatus = Cellular_GetServiceStatus( CellularHandle, &serviceStatus );

            if( ( cellularStatus == CELLULAR_SUCCESS ) &&
                ( ( serviceStatus.psRegistrationStatus == REGISTRATION_STATUS_REGISTERED_HOME) ||
                  ( serviceStatus.psRegistrationStatus == REGISTRATION_STATUS_ROAMING_REGISTERED) ) )
            {
                configPRINTF( ( ">>>  Cellular module registered  <<<\r\n" ) );
                break;
            }
            else
            {
                configPRINTF( ( ">>>  Cellular GetServiceStatus failed %d, ps registration status %d  <<<\r\n",
                                cellularStatus, serviceStatus.psRegistrationStatus ) );
            }

            timeoutCount++;

            if( timeoutCount >= timeoutCountLimit )
            {
                configPRINTF( ( ">>>  Cellular module can't be registered  <<<\r\n" ) );
            }

            vTaskDelay( pdMS_TO_TICKS( CELLULAR_PDN_CONNECT_WAIT_INTERVAL_MS ) );
        }
    }

    if( cellularStatus == CELLULAR_SUCCESS )
    {
        cellularStatus = Cellular_ActivatePdn( CellularHandle, CellularSocketPdnContextId );
    }

    if( cellularStatus == CELLULAR_SUCCESS )
    {
        cellularStatus = Cellular_GetIPAddress( CellularHandle, CellularSocketPdnContextId, localIP, sizeof( localIP ) );
    }

    if( cellularStatus == CELLULAR_SUCCESS )
    {
        cellularStatus = Cellular_GetPdnStatus( CellularHandle, &PdnStatusBuffers, CellularSocketPdnContextId, &NumStatus );
    }

    if( ( cellularStatus == CELLULAR_SUCCESS ) && ( PdnStatusBuffers.state == 1 ) )
    {
        configPRINTF( ( ">>>  Cellular module registered, IP address %s  <<<\r\n", localIP ) );
        cellularRet = true;
    }
    else
    {
        cellularRet = false;
    }

    return cellularRet;
}

/*-----------------------------------------------------------*/