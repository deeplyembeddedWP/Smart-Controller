/*
 * logging_stack.h
 *
 * Created: 19/11/2021 2:27:04 PM
 *  Author: Developer
 */ 


#ifndef LOGGING_STACK_H_
#define LOGGING_STACK_H_

/* Include header for logging level macros. */
#include "logging_levels.h"

/* Standard Include. */
#include <stdint.h>
#include <stdio.h>

/* Logging API to be mapped to */
#include "FreeRTOS.h"
#include "iot_logging_task.h"

/* Metadata information to prepend to every log message. */
#define LOG_METADATA_FORMAT    "[%s:%d] "
#define LOG_METADATA_ARGS      __FUNCTION__, __LINE__

/* Common macro for all logging interface macros. */
#if !defined( DISABLE_LOGGING )
#define SdkLog( string )    vLoggingPrintf string
#else
#define SdkLog( string )
#endif

/* Check that LIBRARY_LOG_LEVEL is defined and has a valid value. */
#if !defined( LIBRARY_LOG_LEVEL ) ||       \
( ( LIBRARY_LOG_LEVEL != LOG_NONE ) && \
( LIBRARY_LOG_LEVEL != LOG_ERROR ) &&  \
( LIBRARY_LOG_LEVEL != LOG_WARN ) &&   \
( LIBRARY_LOG_LEVEL != LOG_INFO ) &&   \
( LIBRARY_LOG_LEVEL != LOG_DEBUG ) )
#error "Please define LIBRARY_LOG_LEVEL as either LOG_NONE, LOG_ERROR, LOG_WARN, LOG_INFO, or LOG_DEBUG."
#elif !defined( LIBRARY_LOG_NAME )
#error "Please define LIBRARY_LOG_NAME for the library."
#else
#if LIBRARY_LOG_LEVEL == LOG_DEBUG
/* All log level messages will logged. */
#define LogError( message )    SdkLog( ( "[ERROR] [%s] "LOG_METADATA_FORMAT, LIBRARY_LOG_NAME, LOG_METADATA_ARGS ) ); SdkLog( message ); SdkLog( ( "\r\n" ) )
#define LogWarn( message )     SdkLog( ( "[WARN] [%s] "LOG_METADATA_FORMAT, LIBRARY_LOG_NAME, LOG_METADATA_ARGS ) ); SdkLog( message ); SdkLog( ( "\r\n" ) )
#define LogInfo( message )     SdkLog( ( "[INFO] [%s] "LOG_METADATA_FORMAT, LIBRARY_LOG_NAME, LOG_METADATA_ARGS ) ); SdkLog( message ); SdkLog( ( "\r\n" ) )
#define LogDebug( message )    SdkLog( ( "[DEBUG] [%s] "LOG_METADATA_FORMAT, LIBRARY_LOG_NAME, LOG_METADATA_ARGS ) ); SdkLog( message ); SdkLog( ( "\r\n" ) )

#elif LIBRARY_LOG_LEVEL == LOG_INFO
/* Only INFO, WARNING and ERROR messages will be logged. */
#define LogError( message )    SdkLog( ( "[ERROR] [%s] "LOG_METADATA_FORMAT, LIBRARY_LOG_NAME, LOG_METADATA_ARGS ) ); SdkLog( message ); SdkLog( ( "\r\n" ) )
#define LogWarn( message )     SdkLog( ( "[WARN] [%s] "LOG_METADATA_FORMAT, LIBRARY_LOG_NAME, LOG_METADATA_ARGS ) ); SdkLog( message ); SdkLog( ( "\r\n" ) )
#define LogInfo( message )     SdkLog( ( "[INFO] [%s] "LOG_METADATA_FORMAT, LIBRARY_LOG_NAME, LOG_METADATA_ARGS ) ); SdkLog( message ); SdkLog( ( "\r\n" ) )
#define LogDebug( message )

#elif LIBRARY_LOG_LEVEL == LOG_WARN
/* Only WARNING and ERROR messages will be logged.*/
#define LogError( message )    SdkLog( ( "[ERROR] [%s] "LOG_METADATA_FORMAT, LIBRARY_LOG_NAME, LOG_METADATA_ARGS ) ); SdkLog( message ); SdkLog( ( "\r\n" ) )
#define LogWarn( message )     SdkLog( ( "[WARN] [%s] "LOG_METADATA_FORMAT, LIBRARY_LOG_NAME, LOG_METADATA_ARGS ) ); SdkLog( message ); SdkLog( ( "\r\n" ) )
#define LogInfo( message )
#define LogDebug( message )

#elif LIBRARY_LOG_LEVEL == LOG_ERROR
/* Only ERROR messages will be logged. */
#define LogError( message )    SdkLog( ( "[ERROR] [%s] "LOG_METADATA_FORMAT, LIBRARY_LOG_NAME, LOG_METADATA_ARGS ) ); SdkLog( message ); SdkLog( ( "\r\n" ) )
#define LogWarn( message )
#define LogInfo( message )
#define LogDebug( message )

#else /* if LIBRARY_LOG_LEVEL == LOG_ERROR */

#define LogError( message )
#define LogWarn( message )
#define LogInfo( message )
#define LogDebug( message )

#endif /* if LIBRARY_LOG_LEVEL == LOG_ERROR */
#endif /* if !defined( LIBRARY_LOG_LEVEL ) || ( ( LIBRARY_LOG_LEVEL != LOG_NONE ) && ( LIBRARY_LOG_LEVEL != LOG_ERROR ) && ( LIBRARY_LOG_LEVEL != LOG_WARN ) && ( LIBRARY_LOG_LEVEL != LOG_INFO ) && ( LIBRARY_LOG_LEVEL != LOG_DEBUG ) ) */

#endif /* ifndef LOGGING_STACK_H_ */
